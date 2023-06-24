/***************************************************
 * file: qinc/rpn-lang/rpn.h
 *
 * @file    rpn.h
 * @author  Eric L. Hernes
 * @born_on   Thursday, May 25, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C/C++ header
 *
 * $Id$
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <cmath>
#include <stdexcept>
#include <functional>

namespace rpn {
  class Stack {
  public:
    class Object {
    public:
      virtual ~Object() {};
      virtual bool operator==(const Object &rhs) const =0;
      virtual bool operator>(const Object &/*rhs*/) const { throw std::runtime_error("operator> invalid for type"); };
      virtual bool operator<(const Object &/*rhs*/) const { throw std::runtime_error("operator> invalid for type"); };
      virtual operator std::string() const =0;
      virtual std::unique_ptr<Object> deep_copy() const =0;
      std::string to_string() const { return static_cast<std::string>(*this); }
    };

    Stack() {};
    ~Stack() {};

    void push(const Object &ob);
    void push_boolean(const bool &val);
    void push_string(const std::string &val);
    void push_integer(const int64_t &val);
    void push_double(const double &val);

    bool pop_boolean();
    std::string pop_string();
    int64_t pop_integer();
    double pop_double();
    double pop_as_double(); // auto-converts integers to double, returns NaN if it couldn't convert
    bool pop_as_boolean(); // auto-converts boolean, integer, double, and string, returns false if it couldn't convert

    std::unique_ptr<Object> pop();

    Object &peek(int n);
    bool peek_boolean(int n);
    std::string peek_string(int n);
    std::string peek_as_string(int n); // auto-converts to string if the type is not string
    int64_t peek_integer(int n);
    double peek_double(int n);
    double peek_as_double(int n); // auto-converts integers to double, returns NaN if it couldn't convert

    // basic stack operations

    void clear(); // [prim]
    size_t depth(); // [prim]
    void dropn(int n); // [prim]
    void dupn(int n); // [prim]
    void nipn(int n); // [prim] drop Nos
    void pick(int n); // [prim] copy Nos to top
    void rolldn(int n); // prim
    void rollun(int n); // prim
    void tuckn(int n); // [prim] copy Tos to n-pos
    void swap(); // [prim] for efficiency, not that it matters in 2023
    void drop(); // [prim] for efficiency, not that it matters in 2023
    void rollu(); // rollu(depth);
    void rolld(); // rollu(depth);

    void over(); // pick(2);
    void dup(); // pick(1);
    void rotu(); // rollu(3);
    void rotd(); // rolld(3);

    void reverse();
    void reversen(int n);

    void print(const std::string &msg="");

    std::vector<size_t> types() const;
  private:
    std::deque<std::unique_ptr<Object>> _stack;
  };

  class Interp;

  // base class to give a word definition context
  class WordContext {
  public:
    WordContext() {}
    virtual ~WordContext() {}
  protected:
  };

  // Class family for validating word definitions against stack type and depth
  class StackValidator {
  public:
    virtual bool operator()(const std::vector<size_t> &types, rpn::Stack &stack) const =0;
  protected:
  };

  class StrictTypeValidator : public StackValidator {
  public:
    static const StrictTypeValidator d1_double;
    static const StrictTypeValidator d1_integer;
    static const StrictTypeValidator d1_boolean;
    static const StrictTypeValidator d1_object;
    static const StrictTypeValidator d1_string;
    static const StrictTypeValidator d1_array;
    static const StrictTypeValidator d1_vec3;

    static const StrictTypeValidator d2_vec3_vec3;
    static const StrictTypeValidator d2_double_double;
    static const StrictTypeValidator d2_double_integer;
    static const StrictTypeValidator d2_integer_double;
    static const StrictTypeValidator d2_integer_integer;
    static const StrictTypeValidator d2_boolean_boolean;

    static const StrictTypeValidator d2_vec3_double;
    static const StrictTypeValidator d2_double_vec3;
    static const StrictTypeValidator d2_vec3_integer;
    static const StrictTypeValidator d2_integer_vec3;

    static const StrictTypeValidator d2_array_any;
    static const StrictTypeValidator d2_any_array;

    static const StrictTypeValidator d2_string_any;
    static const StrictTypeValidator d2_any_string;

    static const StrictTypeValidator d2_object_any;
    static const StrictTypeValidator d2_any_object;

    static const StrictTypeValidator d3_double_double_double;
    static const StrictTypeValidator d3_integer_double_double;
    static const StrictTypeValidator d3_double_integer_double;
    static const StrictTypeValidator d3_double_double_integer;

    static const StrictTypeValidator d3_integer_integer_integer;
    static const StrictTypeValidator d3_double_integer_integer;
    static const StrictTypeValidator d3_integer_double_integer;
    static const StrictTypeValidator d3_integer_integer_double;

    static const StrictTypeValidator d3_any_any_boolean;
    static const StrictTypeValidator d3_object_string_any;
    static const StrictTypeValidator d3_string_any_object;

    static const size_t v_anytype;
    //    static const size_t v_numbertype;  // is harder than it sounds...

    StrictTypeValidator(const std::vector<size_t> &types) : _types(types) {}
    virtual bool operator()(const std::vector<size_t> &types, rpn::Stack &stack) const override;
  private:
    const std::vector<size_t> _types;
  };

  class StackSizeValidator : public StackValidator {
  public:
    static const StackSizeValidator zero;
    static const StackSizeValidator one;
    static const StackSizeValidator two;
    static const StackSizeValidator three;
    static const StackSizeValidator ntos; // n top of stack
    
    StackSizeValidator(size_t n) : _n(n) {}
    virtual bool operator()(const std::vector<size_t> &types, rpn::Stack &stack) const override;
  private:
    size_t _n;
  };

  struct WordDefinition {
    enum class Result {
      ok,
      parse_error, // parsing problem, definition, comment, string-literal, etc
      dict_error, // no such word
      param_error, // parameters not right for the word
      eval_error, // eval went awry
      compile_error, // error in compiling
      implementation_error, // not implmemented or similar
    };
    //    std::string description;
    const StackValidator &validator;
    std::function<Result(Interp &rpn, WordContext *ctx, std::string &rest)> eval;
    WordContext *context;
  };

  class Interp {
  public:
    Interp();
    ~Interp();
    rpn::WordDefinition::Result parse(std::string &line);
    rpn::WordDefinition::Result parseFile(const std::string &path);

    bool addDefinition(const std::string &word, const WordDefinition &def);
    bool addCompiledWord(const std::string &word, const std::string &def, const StackValidator &v = StackSizeValidator::zero);

    bool validateWord(const std::string &word);
    bool wordExists(const std::string &word);

    /*
     * XXX-ELH- should the stack be public or private?
     *
     * Making it public makes it easier for the native words to manipulate it.
     * It is not an implementation detail, but rather an integral part of what
     * it is and how it is intended to be used. 
     */

    Stack stack;
    const std::string &status();

    struct Privates;
  private:
    void addStackWords();
    void addMathWords();
    void addLogicWords();
    void addTypeWords();
    Privates *m_p;
  };
}

#define PEEK_CAST(obtype,ob)  dynamic_cast<obtype&>(ob)
#define POP_CAST(obtype,ob)  dynamic_cast<obtype&>(*ob.get())
#define OBJECTP_CAST(obtype)  dynamic_cast<obtype*>

template<typename T>
class TStackObject : public rpn::Stack::Object {
 public:
  TStackObject() = default; //: _v(v) {}
  TStackObject(const T &v) : _v(v) {}
  virtual bool operator==(const Object &orhs) const override {
    auto *rhs = OBJECTP_CAST(const TStackObject<T>)(&orhs);
    return (rhs !=nullptr && _v.val() == rhs->_v.val());
  }
  virtual bool operator>(const Object &orhs) const override {
    auto &rhs = PEEK_CAST(const TStackObject<T>,orhs);
    return (_v.val() > rhs._v.val());
  }
  virtual bool operator<(const Object &orhs) const override {
    auto &rhs = PEEK_CAST(const TStackObject<T>,orhs);
    return (_v.val() < rhs._v.val());
  }
  virtual ~TStackObject() {}
  virtual std::unique_ptr<rpn::Stack::Object> deep_copy() const override { return std::make_unique<TStackObject<T>>(*this); };
  virtual operator std::string() const override { return (std::string)_v; };
  auto val() const { return _v.val(); };
  auto &inner() { return _v; };
 private:
  T _v;
};

class XDouble {
 public:
 XDouble(const double &v) : _v(v) {}
  virtual operator std::string() const { return std::to_string(_v); };
  auto val() const { return _v; }
  bool operator==(const XDouble &rhs) const {
    return _v == rhs._v;
  }
 private:
  double _v;
};

class XInteger {
 public:
 XInteger(const int64_t &v) : _v(v) {}
  virtual operator std::string() const { return std::to_string(_v); };
  auto val() const { return _v; }
  bool operator==(const XInteger &rhs) const {
    return _v == rhs._v;
  }
 private:
  int64_t _v;
};

class XBoolean {
 public:
 XBoolean(const bool &v) : _v(v) {}
  virtual operator std::string() const { return _v ? "<true>" : "<false>"; };
  auto val() const { return _v; }
  bool operator==(const XBoolean &rhs) const {
    return _v == rhs._v;
  }
 private:
  bool _v;
};

class XString {
 public:
  XString(const XString &x): _v(x._v) {}
  XString(const std::string &v) : _v(v) {}
  virtual operator std::string() const { return _v; };
  auto val() const { return _v; }
  bool operator==(const XString &rhs) const {
    return _v == rhs._v;
  }
 private:
  std::string _v;
};

#include <map>
class XObject {
public:
  XObject() = default;
  XObject(const XObject &v)  {
    for(auto const &m : v._v) {
      _v.emplace(m.first, m.second->deep_copy());
    }
  }
  bool operator==(const XObject &rhs) const {
    bool rv = _v.size() == rhs._v.size();
    for(auto i=_v.cbegin(), j=rhs._v.cbegin(); rv && i!= _v.cend(); i++,j++) {
      rv &= (i->first == j->first) && (*(i->second) == *(j->second));
    }
    return rv;
  }
  void add_value(const std::string &name, const rpn::Stack::Object &val) {
    _v.emplace(name, val.deep_copy());
  }
  bool has_member(const std::string &name) {
    return (_v.find(name) != _v.end());
  }
  rpn::Stack::Object &member(const std::string &name) {
    auto v = _v.find(name);
    if (v != _v.end()) {
      return *v->second;
    } else {
      std::string err = "XObject: no such member (";
      throw std::runtime_error(err + name + ")");
    }
  }
  virtual operator std::string() const {
    std::string rv = "{";
    for(auto const &m : _v) {
      rv += m.first;
      rv += ":";
      rv += m.second->to_string();
      rv += ", ";
    }
    rv += "}";
    return rv;
  };
  const auto &val() const { return _v; };
protected:
  std::map<std::string,std::unique_ptr<rpn::Stack::Object>> _v;
};

class XArray {
public:
  XArray() = default;
  XArray(const XArray &a)  {
    for(auto const &e : a._v) {
      _v.push_back(e->deep_copy());
    }
  }
  bool operator==(const XArray &rhs) const {
    bool rv = _v.size() == rhs._v.size();
    for(auto i=_v.cbegin(), j=rhs._v.cbegin(); rv && i!= _v.cend(); i++,j++) {
      rv &= (*i == *j);
    }
    return rv;
  }
  void add_value(const rpn::Stack::Object &val) {
    _v.push_back(val.deep_copy());
  }
  virtual operator std::string() const {
    std::string rv = "[";
    for(auto const &e : _v) {
      rv += e->to_string();
      rv += ", ";
    }
    rv += "]";
    return rv;
  };
  const auto &val() const { return _v; };
 protected:
  std::vector<std::unique_ptr<rpn::Stack::Object>> _v;
};

using StDouble = TStackObject<XDouble>;
using StInteger = TStackObject<XInteger>;
using StBoolean = TStackObject<XBoolean>;
using StString = TStackObject<XString>;
using StObject = TStackObject<XObject>;
using StArray = TStackObject<XArray>;

class StVec3 : public rpn::Stack::Object {
public:
  StVec3(const StVec3 &other) : _x(other._x), _y(other._y), _z(other._z) {};
  StVec3(const double &x=std::nan(""), const double &y=std::nan(""), const double &z=std::nan("")) : _x(x), _y(y), _z(z) {};
  virtual ~StVec3() {};
  virtual bool operator==(const Object &orhs) const override {
    const StVec3 &rhs = PEEK_CAST(const StVec3,orhs);
    // we might need to include some abs epsilon calculation here
    return (((_x == rhs._x) || (std::isnan(_x) && std::isnan(rhs._x))) &&
	    ((_y == rhs._y) || (std::isnan(_y) && std::isnan(rhs._y))) &&
	    ((_z == rhs._z) || (std::isnan(_z) && std::isnan(rhs._z))));
  };

  //  virtual bool operator>(const Object &rhs) const { throw std::runtime_error("operator> invalid for type"); };
  //  virtual bool operator<(const Object &rhs) const { throw std::runtime_error("operator> invalid for type"); };

  virtual operator std::string() const override {
    std::string rv = "<";
    if (!std::isnan(_x)) {
      rv += " x:";
      rv += std::to_string(_x);
    }
    if (!std::isnan(_y)) {
      rv += " y:";
      rv += std::to_string(_y);
    }
    if (!std::isnan(_z)) {
      rv += " z:";
      rv += std::to_string(_z);
    }
    rv += " >";
    return rv;
  }
  virtual std::unique_ptr<Object> deep_copy() const override { return std::make_unique<StVec3>(*this); }

public:
  // should these be public or private?
  // we'll make them public so that the rpn words can work with them more easily
  double _x;
  double _y;
  double _z;
};

// convenience macros for adding native methods
#define NATIVE_WORD_FN(mangler, op) mangler##_func_##op

#define NATIVE_WORD_DECL(mangler, fn) \
  static rpn::WordDefinition::Result NATIVE_WORD_FN(mangler, fn)(rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest)
   
#define NATIVE_WORD_FN_0_DOUBLE(mangler, fn, val) \
  NATIVE_WORD_DECL(mangler, fn) {					\
    rpn.stack.push_double(val);						\
    return rpn::WordDefinition::Result::ok;				\
  }

#define NATIVE_WORD_FN_0_INTEGER(mangler, fn, val) \
  NATIVE_WORD_DECL(mangler, fn) {					\
    rpn.stack.push_integer(val);					\
    return rpn::WordDefinition::Result::ok;				\
  }

#define NATIVE_WORD_FN_1_NUMBER(mangler, fn)				\
  NATIVE_WORD_DECL(mangler, fn) {					\
    double s1 = rpn.stack.pop_as_double();				\
    rpn.stack.push_double(fn(s1));					\
    return rpn::WordDefinition::Result::ok;				\
  }
  
#define NATIVE_WORD_FN_1_INTEGER(mangler, fn) \
  NATIVE_WORD_DECL(mangler, fn) {					\
    auto s1 = rpn.stack.pop_integer();					\
    rpn.stack.push_integer(fn(s1));					\
    return rpn::WordDefinition::Result::ok;				\
  }
  
#define NATIVE_WORD_FN_2_NUMBER(mangler, fn)				\
  NATIVE_WORD_DECL(mangler, fn) {					\
    auto s1 = rpn.stack.pop_as_double();				\
    auto s2 = rpn.stack.pop_as_double();				\
    rpn.stack.push_double(fn(s2,s1));					\
    return rpn::WordDefinition::Result::ok;				\
  }
  
#define NATIVE_WORD_FN_2_INTEGER(mangler, fn)				\
  NATIVE_WORD_DECL(mangler, fn) {					\
    auto s1 = rpn.stack.pop_integer();					\
    auto s2 = rpn.stack.pop_integer();					\
    rpn.stack.push_integer(fn(s2,s1));					\
    return rpn::WordDefinition::Result::ok;				\
  }

#define NATIVE_WORD_WDEF(mangler, validator, w, ptr)			\
  { validator, NATIVE_WORD_FN(mangler, w), ptr }

// common case for binary function that converts to double except for
// when both parameters are integers
#define ADD_NATIVE_2_NUMBER_WDEF(mangler, r, symbol, double_func, integer_func, ptr) \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d2_double_double, double_func, ptr)); \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d2_double_integer, double_func, ptr)); \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d2_integer_double, double_func, ptr)); \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d2_integer_integer, integer_func, ptr))

#define ADD_NATIVE_1_NUMBER_WDEF(mangler, r, symbol, double_func, integer_func, ptr) \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d1_double, double_func, ptr)); \
  r.addDefinition(symbol, NATIVE_WORD_WDEF(mangler, rpn::StrictTypeValidator::d1_integer, integer_func, ptr))


/* end of qinc/rpn-lang/rpn.h */
