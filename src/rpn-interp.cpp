/***************************************************
 * file: qinc/rpn-lang/src/rpn-interp.cpp
 *
 * @file    rpn-interp.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Saturday, May 27, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include <fstream>

#include "../rpn.h"

std::string
rpn::to_string(const double &dv) {
  char tmp[32];
  snprintf(tmp, sizeof(tmp), "%0.4f", dv);
  return tmp;
}

static std::string::size_type
nextWord(std::string &word, std::string &buffer, const std::string &delim=" \n") {
  word = "";
  auto p1 = buffer.find_first_of(delim, 0);
  if (p1 == std::string::npos) { // not found
    word = buffer;
    buffer = "";
  } else {
    word = buffer.substr(0, p1);
    buffer = buffer.substr(p1+1, std::string::npos);
  }
  return p1;
}

using var_dict_t = std::map<std::string,std::unique_ptr<rpn::Stack::Object>>;

enum CompileType {
  ct_worddef,
  ct_forloop,
  ct_whileloop,
  ct_lambda,
  ct_mathexpr
};

struct Progn : public rpn::WordContext, public rpn::Stack::Object {
public:
  Progn(rpn::Interp::Privates &p, CompileType t) : _p(p), _type(t) { _locals = std::make_shared<var_dict_t>(); };
  Progn(const Progn &other) : _p(other._p), _wordlist(other._wordlist), _type(other._type), _ident(other._ident) {
    _locals = std::make_shared<var_dict_t>();
    for(auto const &v : *other._locals) {
      _locals->emplace(v.first, v.second->deep_copy());
    }
  }

  virtual bool operator==(const Object &orhs) const override {
    auto &rhs = PEEK_CAST(const Progn,orhs);
    return ((_type == rhs._type) &&
	    (_wordlist == rhs._wordlist) &&
	    (_locals == rhs._locals));
  }

  virtual operator std::string() const override {
    std::string rv = "<<";
    for(auto const &w : _wordlist) {
      rv += " ";
      rv += w;
    }
    rv += ">>";
    return rv;
  };
  virtual std::unique_ptr<rpn::Stack::Object> deep_copy() const override { return std::make_unique<Progn>(*this); };

  void addWord(const std::string &word) { _wordlist.push_back(word); };

  rpn::WordDefinition::Result eval(rpn::Interp &rpn);

  rpn::WordDefinition::Result eval_forloop(rpn::Interp &rpn);
  rpn::WordDefinition::Result eval_whileloop(rpn::Interp &rpn);
  rpn::WordDefinition::Result eval_lambda(rpn::Interp &rpn);
  rpn::WordDefinition::Result eval_mathexpr(rpn::Interp &rpn);

  const std::vector<std::string> &wordlist() const { return _wordlist; };

  void clear() { _wordlist.clear(); };

  void print() {
    std::string str = (std::string)(*this);
    printf("Progn %s\n", str.c_str());
    printf("  type: %d\n", _type);
    printf("  ident: %s\n", _ident.c_str());
    printf("  locals:");
    for(const auto &lv : *_locals) {
      printf(" %s,", lv.first.c_str());
    }
    printf("\n");
  }
    
  rpn::Interp::Privates &_p;
  std::vector<std::string> _wordlist;
  std::shared_ptr<var_dict_t> _locals;
  CompileType _type;
  std::string _ident; // value and usage depends on type
};

struct rpn::Interp::Privates : public rpn::WordContext {
  Privates() : _tracing(false) {};
  ~Privates() {};

  rpn::WordDefinition::Result eval(rpn::Interp &rpn, const std::string &word, std::string &rest);
  rpn::WordDefinition::Result runtime_eval(rpn::Interp &rpn, const std::string &word, std::string &rest);
  rpn::WordDefinition::Result compiletime_eval(rpn::Interp &rpn, const std::string &word, std::string &rest);

  // add words that require acces to the Privates struct.
  void add_private_words(rpn::Interp &rpn);

  // validates a word in the dictionary and returns an iterator to it (or _rtDictionary.end() )
  std::multimap<std::string,WordDefinition>::iterator validate_word(const std::string &word, rpn::Stack &stack);
  bool word_exists(const std::string &word);


  rpn::WordDefinition::Result start_compile(CompileType t, bool needIdent);
  rpn::WordDefinition::Result end_compile(Progn *&progp, CompileType t);

  bool is_local_variable(const std::string &word);
  bool find_local_variable(var_dict_t::const_iterator &var, const std::string &word);

  /*
   */
  std::multimap<std::string,WordDefinition> _rtDictionary;
  std::map<std::string,WordDefinition> _ctDictionary;

  std::string _status;

  std::vector<Progn> _ctVprogn;
  std::vector<std::shared_ptr<var_dict_t>> _vlocals;

  bool _needIdent;
  bool _tracing;

};

rpn::WordDefinition::Result
Progn::eval_forloop(rpn::Interp &rpn) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double end = rpn.stack.pop_as_double();
  double start = rpn.stack.pop_as_double();
  _p._vlocals.push_back(_locals);
  for(; rv==rpn::WordDefinition::Result::ok && start<end; start += 1) {
    (*_locals)[_ident] = std::make_unique<StDouble>(StDouble(start));
    rv = eval_lambda(rpn);
  }
  _p._vlocals.pop_back();
  return rv;
}

rpn::WordDefinition::Result
Progn::eval_whileloop(rpn::Interp &rpn) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}

rpn::WordDefinition::Result
Progn::eval_lambda(rpn::Interp &rpn) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  std::string rest;

  _p._vlocals.push_back(_locals);

  for(auto wi= _wordlist.cbegin(); rv==rpn::WordDefinition::Result::ok && wi != _wordlist.cend(); wi++) {
    var_dict_t::const_iterator lv = _locals->find(*wi);
    bool lvp = (lv != _locals->end());
    if (!lvp) {
      lvp = _p.find_local_variable(lv, *wi);
    }

    if (lvp) {
      auto *pn = dynamic_cast<Progn*>(&(*lv->second));
      if (pn != nullptr) {
	rpn.stack.print("recursive progn");
	pn->print();
	pn->eval(rpn);

      } else {
	std::string sv = (*lv->second);
	printf("push local: %s => %s\n", lv->first.c_str(), sv.c_str());
	rpn.stack.push(*lv->second);

      }

    } else {
      rv = _p.eval(rpn, *wi, rest);

    }
  }

  _p._vlocals.pop_back();

  return rv;
}

rpn::WordDefinition::Result
Progn::eval_mathexpr(rpn::Interp &rpn) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}

rpn::WordDefinition::Result
Progn::eval(rpn::Interp &rpn) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  switch (_type) {
  case ct_worddef:
    rv = eval_lambda(rpn);
    break;

  case ct_forloop:
    rv = eval_forloop(rpn);
    break;

  case ct_whileloop:
    rv = eval_whileloop(rpn);
    break;

  case ct_lambda:
    rv = eval_lambda(rpn);
    break;

  case ct_mathexpr:
    rv = eval_mathexpr(rpn);
    break;
  }
  return rv;
}

NATIVE_WORD_DECL(private, COMPILED_EVAL)  {
  Progn *progn = dynamic_cast<Progn*>(ctx);
  //  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  rpn::WordDefinition::Result rv = (progn) ? rpn::WordDefinition::Result::ok : rpn::WordDefinition::Result::eval_error;
  rv = progn->eval(rpn);
  return rv;
}

NATIVE_WORD_DECL(private, COLON) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  return p->start_compile(ct_worddef, true);
}

NATIVE_WORD_DECL(private, ct_SEMICOLON) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);

  Progn *progp=nullptr;
  rpn::WordDefinition::Result rv = p->end_compile(progp, ct_worddef);
  if (rv == rpn::WordDefinition::Result::ok) {

    if (p->_tracing) {
      printf("adding '%s' to the dictionary\n", progp->_ident.c_str());
    }

    p->_rtDictionary.emplace(progp->_ident, rpn::WordDefinition {
	rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, COMPILED_EVAL), progp });

  } else {

    rv = rpn::WordDefinition::Result::compile_error;
  }
  return rv;
}

NATIVE_WORD_DECL(private, TRACE) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  bool pred = rpn.stack.pop_as_boolean();
  p->_tracing = pred;
  return rv;
}

NATIVE_WORD_DECL(private, BOOL_TRUE) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn.stack.push_boolean(true);
  return rv;
}

NATIVE_WORD_DECL(private, BOOL_FALSE) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn.stack.push_boolean(false);
  return rv;
}

NATIVE_WORD_DECL(private, OPAREN) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  std::string comment;
  auto cp = nextWord(comment, rest, ")");
  if (cp == std::string::npos) {
    rest = comment; // reset the buffer for future error message
    rv = rpn::WordDefinition::Result::parse_error;
  }
  return rv;
}

NATIVE_WORD_DECL(private, DQUOTE) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  std::string literal;
  auto pos = nextWord(literal, rest, "\"");
  if (pos != std::string::npos) {
    rpn.stack.push_string(literal);
  } else {
    rv = rpn::WordDefinition::Result::parse_error;
    rest = literal; // reset buffer for error messages and diagnostics
  }
  return rv;
}

NATIVE_WORD_DECL(private, ct_DQUOTE) {
  // (rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  std::string literal;
  auto pos = nextWord(literal, rest, "\"");
  if (pos != std::string::npos) {
    p->_ctVprogn.back().addWord(".\"");
    p->_ctVprogn.back().addWord(literal);
  } else {
    rv = rpn::WordDefinition::Result::parse_error;
    rest = literal; // reset buffer for error messages and diagnostics
  }
  return rv;
}

NATIVE_WORD_DECL(private, FOR) {
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  return p->start_compile(ct_forloop, true);
}

NATIVE_WORD_DECL(private, ct_FOR) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  //  if (p->_ctVprogn.back()._type == ct_worddef) {
  //    p->_ctVprogn.back().addWord("FOR");
  //  } else {
    rv = p->start_compile(ct_forloop, true);
    //  }
  return rv;
}

rpn::WordDefinition::Result
rpn::Interp::Privates::start_compile(CompileType t, bool needIdent) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  _needIdent = needIdent;
  _ctVprogn.push_back(Progn(*this, t));
  return rv;
}

rpn::WordDefinition::Result
rpn::Interp::Privates::end_compile(Progn *&progp, CompileType t) {
  rpn::WordDefinition::Result rv = ((_ctVprogn.size()>0) && _ctVprogn.back()._type == t)?
    rpn::WordDefinition::Result::ok : rpn::WordDefinition::Result::compile_error;

  progp=nullptr;
  if (rv == rpn::WordDefinition::Result::ok) {
    progp = new Progn(_ctVprogn.back());
    _ctVprogn.pop_back();
  }

  return rv;
}

NATIVE_WORD_DECL(private, ct_NEXT) {
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;

  /*  if (p->_ctVprogn.back()._type == ct_worddef) {
    p->_ctVprogn.back().addWord("NEXT");

    } else */ {
    Progn *progp=nullptr;

    rv = p->end_compile(progp, ct_forloop);

    if (rv == rpn::WordDefinition::Result::ok) {

      if (p->_ctVprogn.size() == 0) {
	// back to top level, evaluate here
	rv = progp->eval(rpn);

	delete progp;
	//	p->_locals.clear();

      } else {

	// in a definition or nested loops

	std::string word = std::to_string((uint64_t)progp);
	p->_ctVprogn.back()._locals->emplace(word, progp);
	//	delete progp; who owns progp???
	p->_ctVprogn.back().addWord(word);
      }

    } else {

      rv = rpn::WordDefinition::Result::compile_error;
    }
  }

  return rv;
}

NATIVE_WORD_DECL(private, ct_STEP) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::Interp::Privates *p = dynamic_cast<rpn::Interp::Privates*>(ctx);
  auto step = rpn.stack.pop_integer();
  return rv;
}

void
rpn::Interp::Privates::add_private_words(rpn::Interp &rpn) {
  rpn.addDefinition(":", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, COLON), this });
  rpn.addDefinition("(", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, OPAREN), this });
  rpn.addDefinition(".\"", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, DQUOTE), this });
  rpn.addDefinition("FOR", { rpn::StrictTypeValidator::d2_integer_integer, NATIVE_WORD_FN(private, FOR), this });
  rpn.addDefinition("TRACE", { rpn::StrictTypeValidator::d1_boolean, NATIVE_WORD_FN(private, TRACE), this });

  rpn.addDefinition("<true>", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, BOOL_TRUE), this });
  rpn.addDefinition("<false>", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, BOOL_FALSE), this });

  _ctDictionary.emplace(";", rpn::WordDefinition { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, ct_SEMICOLON), this });
  _ctDictionary.emplace("(", rpn::WordDefinition { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, OPAREN), this });
  _ctDictionary.emplace(".\"", rpn::WordDefinition { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, ct_DQUOTE), this });
  _ctDictionary.emplace("FOR", rpn::WordDefinition { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, ct_FOR), this });
  _ctDictionary.emplace("NEXT", rpn::WordDefinition { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(private, ct_NEXT), this });
  _ctDictionary.emplace("STEP", rpn::WordDefinition { rpn::StrictTypeValidator::d1_double, NATIVE_WORD_FN(private, ct_STEP), this });
}

rpn::WordDefinition::Result
rpn::Interp::Privates::eval(rpn::Interp &rpn, const std::string &word, std::string &rest) {
  if (_tracing)
    printf("evaluating: '%s' '%s'\n", word.c_str(), rest.c_str());

  if (word.size()==0) {
    return rpn::WordDefinition::Result::ok;
  }

  rpn::WordDefinition::Result rv=rpn::WordDefinition::Result::eval_error;
  std::string wstatus  = word + ": ";

  std::string msg;
  if (_ctVprogn.size() != 0) {
    try {
      rv = compiletime_eval(rpn,word,rest);

    } catch (const std::bad_cast &bce) {
      msg = "type error compiling";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
      rv = rpn::WordDefinition::Result::param_error;
      _ctVprogn.clear();

    } catch (const std::runtime_error &rte) {
      rv = rpn::WordDefinition::Result::eval_error;
      msg = "eval error compiling";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
      _ctVprogn.clear();
    }

  } else {
    try {
      rv = runtime_eval(rpn,word,rest);

    } catch (const std::bad_cast &bce) {
      rv = rpn::WordDefinition::Result::param_error;
      msg = "type error";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");

    } catch (const std::runtime_error &rte) {
      rv = rpn::WordDefinition::Result::param_error;
      msg = "eval error";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
    }
  }

  if (msg == "") {
    switch (rv) {
    case rpn::WordDefinition::Result::ok: {
      // crickets
      msg = "ok";
    }
      break;

    case rpn::WordDefinition::Result::parse_error: {
      msg = "parse error ";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
      rest = ""; // discard the rest of the line;
    }
      break;

    case rpn::WordDefinition::Result::dict_error: {
      msg = "not found";
    }
      break;

    case rpn::WordDefinition::Result::param_error: {
      msg = "parameter error";
    }
      break;

    case rpn::WordDefinition::Result::implementation_error: {
      msg = "implementation error";
    }
      break;

    case rpn::WordDefinition::Result::eval_error: {
      msg = "eval error";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
    }
      break;

    case rpn::WordDefinition::Result::compile_error: {
      msg = "compile error";
      if (rest.size()>0) msg += (std::string(" '") + rest + "'");
    }
      break;
    }
  }

  _status = wstatus + msg;

  if (rv != rpn::WordDefinition::Result::ok) {
    printf("%s: %s\n", __func__, _status.c_str());
  }

  if (_tracing)
    printf("returns: %d (%s)\n", rv, rest.c_str());
   
  return rv;
}

rpn::WordDefinition::Result
rpn::Interp::Privates::runtime_eval(rpn::Interp &rpn, const std::string &word, std::string &rest) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::dict_error;
  // numbers just push
  if (std::isdigit(word[0])||(word[0]=='-'&&std::isdigit(word[1]))) {
    if (word.find('.') != std::string::npos) {
      double val = strtod(word.c_str(), nullptr);
      rpn.stack.push_double(val);
    } else {
      long val = strtol(word.c_str(), nullptr, 0);
      rpn.stack.push_integer(val);
    }
    rv = rpn::WordDefinition::Result::ok;
  } else {
    if (word_exists(word)) {
      auto we = validate_word(word, rpn.stack);
      if (we != _rtDictionary.end()) {
	rv = we->second.eval(rpn,  we->second.context, rest);
      } else {
	rv = rpn::WordDefinition::Result::param_error;
      }
    } else {
      // default to dictionary error
    }
  }
  return rv;
}

bool
rpn::Interp::Privates::is_local_variable(const std::string &word) {
  bool rv = false;
  for(auto  pn =_ctVprogn.cbegin(); rv==false && pn != _ctVprogn.cend(); pn++) {
    rv = (rv || (word == pn->_ident) || (pn->_locals->find(word)!=pn->_locals->end()));
  }
  return rv;
}

bool
rpn::Interp::Privates::find_local_variable(var_dict_t::const_iterator &var, const std::string &word) {
  bool rv = false;
  for(auto  pn =_vlocals.crbegin(); rv==false && pn != _vlocals.crend(); pn++) {
    var = (*pn)->find(word);
    rv = (var != (*pn)->end());
  }
  return rv;
}

rpn::WordDefinition::Result
rpn::Interp::Privates::compiletime_eval(rpn::Interp &rpn, const std::string &word, std::string &rest) {
  rpn::WordDefinition::Result rv=rpn::WordDefinition::Result::dict_error;
  auto &progn = _ctVprogn.back();
  
  if (_needIdent && (progn._ident=="")) {
    progn._ident = word;
    _needIdent = false;

    rv=rpn::WordDefinition::Result::ok;

  } else {
    const auto &cw = _ctDictionary.find(word);
    if (cw != _ctDictionary.end()) {
      // found something in the compiletime dict, evaluate it
      rv = cw->second.eval(rpn, cw->second.context, rest);

    } else if (std::isdigit(word[0])) {
      // numbers just push
      progn.addWord(word);
      rv=rpn::WordDefinition::Result::ok;

    } else if (is_local_variable(word)) {
      // if we're in a loop compiling mode; and this is the loop variable,
      // push it
      progn.addWord(word);
      rv=rpn::WordDefinition::Result::ok;
      
    } else {
      // everything else, we check in the runtime dictionary
      const auto &rw = _rtDictionary.find(word);
      if (rw != _rtDictionary.end()) {
	progn.addWord(word);
	rv=rpn::WordDefinition::Result::ok;

	//      } else if ((progn._type == ct_worddef) && progn._wordlist.back()=="FOR") {
	// if it's a for-loop, we need the iteration var
	//	progn.addWord(word);

      } else {
	rv = rpn::WordDefinition::Result::dict_error;
	printf("unrecognized word at compile time: '%s'\n", word.c_str());

      }
    }
  }
  return rv;
}

rpn::Interp::Interp() {
  m_p = new Privates;
  m_p->add_private_words(*this);
  addStackWords();
  addLogicWords();
  addMathWords();
  addTypeWords();
}

rpn::Interp::~Interp() {
  if (m_p) delete m_p;
}

const std::string &
rpn::Interp::status() {
  return m_p->_status;
}

bool
rpn::Interp::addDefinition(const std::string &word, const WordDefinition &def) {
  m_p->_rtDictionary.emplace(word, def);
  return true;
}

bool
rpn::Interp::removeDefinition(const std::string &word) {
  m_p->_rtDictionary.erase(word);
  return true;
}

std::multimap<std::string,rpn::WordDefinition>::iterator
rpn::Interp::Privates::validate_word(const std::string &word, rpn::Stack &stack) {
  const auto &beg = _rtDictionary.lower_bound(word);
  const auto &end = _rtDictionary.upper_bound(word);
  if (beg != end) {
    auto stack_types = stack.types();
    for(auto we=beg; we!=end; we++) {
      if (we->second.validator(stack_types, stack)) {
	return we;
      }
    }
  }
  return _rtDictionary.end();
}

bool
rpn::Interp::Privates::word_exists(const std::string &word) {
  auto beg = _rtDictionary.lower_bound(word);
  const auto &end = _rtDictionary.upper_bound(word);
  return (beg != end);
}

bool
rpn::Interp::validateWord(const std::string &word) {
  return m_p->validate_word(word, this->stack) != m_p->_rtDictionary.end();
}

bool
rpn::Interp::wordExists(const std::string &word) {
  return m_p->word_exists(word);
}

rpn::WordDefinition::Result
rpn::Interp::parse(std::string &line) {
  rpn::WordDefinition::Result rv=rpn::WordDefinition::Result::ok;
  for(; rv==rpn::WordDefinition::Result::ok && line.size()>0;) {
    std::string word;
    auto p1 = nextWord(word,line);
    rv = m_p->eval(*this, word, line);
  }
  return rv;
}

rpn::WordDefinition::Result
rpn::Interp::parseFile(const std::string &path) {
  rpn::WordDefinition::Result rv=rpn::WordDefinition::Result::ok;
  std::ifstream ifs(path);

  std::string tmp;
  std::vector<std::string> lines;
  while(getline(ifs, tmp, '\n')) {
    lines.push_back(tmp);
  }

  int lineNo=0;
  for(auto line = lines.begin(); line!=lines.end() && rv==rpn::WordDefinition::Result::ok; line++, lineNo++) {
    rv = parse(*line);
    if (rv != rpn::WordDefinition::Result::ok) {
      printf("parse error at %s:%d\n", path.c_str(), lineNo);
    }
  }

  return rv;
}

/*
 */

bool
rpn::StrictTypeValidator::operator()(const std::vector<size_t> &types, rpn::Stack &stack) const {
  bool rv = types.size() >= _types.size();
  for(auto si=types.cbegin(), wi=_types.cbegin(); rv==true && wi!=_types.cend(); si++, wi++) {
    rv &= ((*wi==v_anytype) || (*si == *wi));
  }
  return rv;
}

bool
rpn::StackSizeValidator::operator()(const std::vector<size_t> &types, rpn::Stack &stack) const {
  bool rv = false;
  if ((_n==(size_t)-1) && types.size()>0 && types[0]==typeid(StInteger).hash_code()) { // negative means to ntos - check top of stack as integer and make sure that the stack is >=
    auto &nn = dynamic_cast<const StInteger&>(stack.peek(1));
    rv = (types.size()-1) >= nn.val();
  } else {
    rv = (types.size() >=_n);
  }
  return rv;
}

/***************************************************
 * canned validators for common stack depth/types
 *
 */
const size_t rpn::StrictTypeValidator::v_anytype = typeid(rpn::Stack::Object).hash_code();
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_double({typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_integer({typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_boolean({typeid(StBoolean).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_string({typeid(StString).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_vec3({typeid(StVec3).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_object({typeid(StObject).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d1_array({typeid(StArray).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_boolean_boolean({typeid(StBoolean).hash_code(), typeid(StBoolean).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_double_double({typeid(StDouble).hash_code(), typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_double_integer({typeid(StDouble).hash_code(), typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_integer_double({typeid(StInteger).hash_code(), typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_integer_integer({typeid(StInteger).hash_code(), typeid(StInteger).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_double_vec3({typeid(StDouble).hash_code(), typeid(StVec3).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_vec3_double({typeid(StVec3).hash_code(), typeid(StDouble).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_integer_vec3({typeid(StInteger).hash_code(), typeid(StVec3).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_vec3_integer({typeid(StVec3).hash_code(), typeid(StInteger).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_vec3_vec3({typeid(StVec3).hash_code(), typeid(StVec3).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_string_any({typeid(StString).hash_code(),rpn::StrictTypeValidator::v_anytype});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_any_string({rpn::StrictTypeValidator::v_anytype,typeid(StString).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_array_any({typeid(StArray).hash_code(), rpn::StrictTypeValidator::v_anytype});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_any_array({rpn::StrictTypeValidator::v_anytype,typeid(StArray).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_object_any({typeid(StObject).hash_code(),rpn::StrictTypeValidator::v_anytype});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d2_any_object({rpn::StrictTypeValidator::v_anytype,typeid(StObject).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_double_double_double({typeid(StDouble).hash_code(),typeid(StDouble).hash_code(),typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_integer_double_double({typeid(StInteger).hash_code(),typeid(StDouble).hash_code(),typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_double_integer_double({typeid(StDouble).hash_code(),typeid(StInteger).hash_code(),typeid(StDouble).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_double_double_integer({typeid(StDouble).hash_code(),typeid(StDouble).hash_code(),typeid(StInteger).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_integer_integer_integer({typeid(StInteger).hash_code(),typeid(StInteger).hash_code(),typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_double_integer_integer({typeid(StDouble).hash_code(),typeid(StInteger).hash_code(),typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_integer_double_integer({typeid(StInteger).hash_code(),typeid(StDouble).hash_code(),typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_integer_integer_double({typeid(StInteger).hash_code(),typeid(StInteger).hash_code(),typeid(StDouble).hash_code()});

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_object_string_any({typeid(StObject).hash_code(),typeid(StString).hash_code(),rpn::StrictTypeValidator::v_anytype});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_string_any_object({typeid(StString).hash_code(),rpn::StrictTypeValidator::v_anytype,typeid(StObject).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d3_any_any_boolean({rpn::StrictTypeValidator::v_anytype, rpn::StrictTypeValidator::v_anytype, typeid(StBoolean).hash_code()} );

const rpn::StrictTypeValidator rpn::StrictTypeValidator::d4_double_double_double_integer({typeid(StDouble).hash_code(),typeid(StDouble).hash_code(),typeid(StDouble).hash_code(),typeid(StInteger).hash_code()});
const rpn::StrictTypeValidator rpn::StrictTypeValidator::d4_integer_double_double_double({typeid(StInteger).hash_code(),typeid(StDouble).hash_code(),typeid(StDouble).hash_code(),typeid(StDouble).hash_code()});

const rpn::StackSizeValidator rpn::StackSizeValidator::zero(0);
const rpn::StackSizeValidator rpn::StackSizeValidator::one(1);
const rpn::StackSizeValidator rpn::StackSizeValidator::two(2);
const rpn::StackSizeValidator rpn::StackSizeValidator::three(3);
const rpn::StackSizeValidator rpn::StackSizeValidator::ntos(-1); // n top of stack

/* end of qinc/rpn-lang/src/rpn-interp.cpp */
