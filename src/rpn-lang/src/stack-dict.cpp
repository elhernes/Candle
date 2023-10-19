/***************************************************
 * file: qinc/rpn-lang/src/stack-dict.cpp
 *
 * @file    stack-dict.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Monday, June 12, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include "../rpn.h"

/* */

#define STACK_OP(op) NATIVE_WORD_FN(stack,op)

#define STACK_OP_FUNC(op)							\
  static rpn::WordDefinition::Result STACK_OP(op)(rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest) { \
    rpn.stack.op();							\
    return rpn::WordDefinition::Result::ok;				\
  }

#define STACK_OPn_FUNC(op)						\
  static rpn::WordDefinition::Result STACK_OP(op)(rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest) { \
    int n = (int)rpn.stack.pop_integer();					\
    rpn.stack.op(n);							\
    return rpn::WordDefinition::Result::ok;				\
  }

STACK_OP_FUNC(drop);
STACK_OP_FUNC(clear);
STACK_OP_FUNC(swap);
STACK_OP_FUNC(rollu);
STACK_OP_FUNC(rolld);
STACK_OP_FUNC(over);
STACK_OP_FUNC(dup);
STACK_OP_FUNC(rotu);
STACK_OP_FUNC(rotd);
STACK_OP_FUNC(print);
STACK_OP_FUNC(reverse);

STACK_OPn_FUNC(dropn);
STACK_OPn_FUNC(dupn);
STACK_OPn_FUNC(nipn);
STACK_OPn_FUNC(pick);
STACK_OPn_FUNC(rolldn);
STACK_OPn_FUNC(rollun);
STACK_OPn_FUNC(tuckn);
STACK_OPn_FUNC(reversen);

// depth is special because we push the value back on the stack
static rpn::WordDefinition::Result STACK_OP(depth)(rpn::Interp &rpn, rpn::WordContext *ctx, std::string &rest) {
  rpn.stack.push_integer(rpn.stack.depth());
  return rpn::WordDefinition::Result::ok;
}

#define ADD_STACK_OP(r, symbol, vv, func)				\
  r.addDefinition(symbol, NATIVE_WORD_WDEF(stack, rpn::StackSizeValidator::vv, func, nullptr))

void
rpn::Interp::addStackWords() {
  rpn::Interp &rpn(*this);

  ADD_STACK_OP(rpn, "DROP", one, drop);
  ADD_STACK_OP(rpn, "CLEAR", zero, clear);
  ADD_STACK_OP(rpn, "DEPTH", zero, depth);
  ADD_STACK_OP(rpn, "SWAP", two, swap);
  ADD_STACK_OP(rpn, "ROLLU", zero, rollu);
  ADD_STACK_OP(rpn, "ROLLD", zero, rolld);
  ADD_STACK_OP(rpn, "OVER", two, over);
  ADD_STACK_OP(rpn, "DUP", one, dup);
  ADD_STACK_OP(rpn, "ROTU", three, rotu);
  ADD_STACK_OP(rpn, "ROTD", three, rotd);
  ADD_STACK_OP(rpn, "DROPn", ntos, dropn);
  ADD_STACK_OP(rpn, "DUPn", ntos, dupn);
  ADD_STACK_OP(rpn, "NIPn", ntos, nipn);
  ADD_STACK_OP(rpn, "PICK", ntos, pick);
  ADD_STACK_OP(rpn, "ROLLDn", ntos, rolldn);
  ADD_STACK_OP(rpn, "ROLLUn", ntos, rollun);
  ADD_STACK_OP(rpn, "TUCKn", ntos, tuckn);
  ADD_STACK_OP(rpn, ".S", zero, print);
  ADD_STACK_OP(rpn, "REVERSE", zero, reverse);
  ADD_STACK_OP(rpn, "REVERSEn", ntos, reversen);

  auto st = rpn.sync_eval(R"(
: DUP2 2 DUPn ;
: DROP2 DROP DROP ;
)");

}

/* end of qinc/rpn-lang/src/stack-dict.cpp */
