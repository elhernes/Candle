/***************************************************
 * file: qinc/rpn-lang/src/logic-dict.cpp
 *
 * @file    logic-dict.cpp
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

NATIVE_WORD_DECL(logic, ifte) {
  bool s1 = rpn.stack.pop_boolean();
  rpn.stack.nipn(s1 ? 1 : 2);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, equal) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(*s1 == *s2);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, not_equal) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(!(*s1 == *s2));
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, greater) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(*s2 > *s1);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, greater_eq) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(!(*s2 < *s1));
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, less) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(*s2 < *s1);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, less_eq) {
  auto s1 = rpn.stack.pop();
  auto s2 = rpn.stack.pop();
  rpn.stack.push_boolean(!(*s2 > *s1));
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, l_not) {
  auto s1 = rpn.stack.pop_boolean();
  rpn.stack.push_boolean(!s1);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, l_and) {
  auto s1 = rpn.stack.pop_boolean();
  auto s2 = rpn.stack.pop_boolean();
  rpn.stack.push_boolean(s1 && s2);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, l_or) {
  auto s1 = rpn.stack.pop_boolean();
  auto s2 = rpn.stack.pop_boolean();
  rpn.stack.push_boolean(s1 || s2);
  return rpn::WordDefinition::Result::ok;
}

NATIVE_WORD_DECL(logic, b_or) {
  auto s1 = rpn.stack.pop_integer();
  auto s2 = rpn.stack.pop_integer();
  rpn.stack.push_integer(s1 | s2);
  return rpn::WordDefinition::Result::ok;
}
NATIVE_WORD_DECL(logic, b_and) {
  auto s1 = rpn.stack.pop_integer();
  auto s2 = rpn.stack.pop_integer();
  rpn.stack.push_integer(s1 & s2);
  return rpn::WordDefinition::Result::ok;
}
NATIVE_WORD_DECL(logic, b_xor) {
  auto s1 = rpn.stack.pop_integer();
  auto s2 = rpn.stack.pop_integer();
  rpn.stack.push_integer(s1 ^ s2);
  return rpn::WordDefinition::Result::ok;
}
NATIVE_WORD_DECL(logic, b_neg) {
  auto s1 = rpn.stack.pop_integer();
  rpn.stack.push_integer(~s1);
  return rpn::WordDefinition::Result::ok;
}

void
rpn::Interp::addLogicWords() {
  //    IF
  //    IFTE
  //    EQ?
  addDefinition("IFTE", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d3_any_any_boolean, ifte, nullptr));
  addDefinition("==", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, equal, nullptr));
  addDefinition(">", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, greater, nullptr));
  addDefinition(">=", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, greater_eq, nullptr));
  addDefinition("<", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, less, nullptr));
  addDefinition("<=", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, less_eq, nullptr));
  addDefinition("!=", NATIVE_WORD_WDEF(logic, rpn::StackSizeValidator::two, not_equal, nullptr));

  addDefinition("NOT", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d1_boolean, l_not, nullptr));
  addDefinition("AND", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d2_boolean_boolean, l_and, nullptr));
  addDefinition("OR", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d2_boolean_boolean, l_or, nullptr));

  addDefinition("NEG", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d1_integer, b_neg, nullptr));
  addDefinition("AND", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d2_integer_integer, b_and, nullptr));
  addDefinition("OR", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d2_integer_integer, b_or, nullptr));
  addDefinition("XOR", NATIVE_WORD_WDEF(logic, rpn::StrictTypeValidator::d2_integer_integer, b_xor, nullptr));
}


/* end of qinc/rpn-lang/src/logic-dict.cpp */
