/***************************************************
 * file: QInc/Projects/color-calc/src/libs/rpn-lang/src/keypad-dict.cpp
 *
 * @file    keypad-dict.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Wednesday, July 26, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include "../rpn.h"

static const rpn::StrictTypeValidator skAssignValidator({
    typeid(StString).hash_code(), typeid(StString).hash_code(),typeid(StInteger).hash_code(), typeid(StInteger).hash_code()
      });

NATIVE_WORD_DECL(keypad, ASSIGN_KEY) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::KeypadController *kc = dynamic_cast<rpn::KeypadController*>(ctx);
  auto label = rpn.stack.pop_string();
  auto word = rpn.stack.pop_string();
  auto column = rpn.stack.pop_integer();
  auto row = rpn.stack.pop_integer();

  if(rpn.wordExists(word)) {
    kc->assignButton(column, row, word, label);
  } else {
    rv = rpn::WordDefinition::Result::eval_error;
  }
  return rv;
}

NATIVE_WORD_DECL(keypad, MATH_KEYS) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::KeypadController *kc = dynamic_cast<rpn::KeypadController*>(ctx);
  kc->clearAssignedButtons();

  kc->assignButton(1,2, "HYPOT");
  kc->assignButton(1,3, "ATAN2");
  kc->assignButton(1,4, "MIN");
  kc->assignButton(1,5, "MAX");
  kc->assignButton(1,6, "INV");
  kc->assignButton(1,7, "SQ");
  kc->assignButton(1,8, "SQRT");
  kc->assignButton(1,9, "^", "Y^X");

  kc->assignButton(2,2, "COS");
  kc->assignButton(2,3, "SIN");
  kc->assignButton(2,4, "TAN");

  kc->assignButton(3,2, "ACOS");
  kc->assignButton(3,3, "ASIN");
  kc->assignButton(3,4, "ATAN");

  kc->assignButton(2,5, "EXP");
  kc->assignButton(2,6, "LN");
  kc->assignButton(2,7, "LN2");
  kc->assignButton(2,8, "LOG");
  kc->assignButton(2,9, "CHS");

  kc->assignButton(3,5,"ROUND");
  kc->assignButton(3,6,"CEIL");
  kc->assignButton(3,7, "FLOOR");
  kc->assignButton(3,8, "k_PI");
  kc->assignButton(3,9, "k_E");
  kc->assignButton(3,10, "RAND");
  kc->assignButton(3,11, "RAND48");
  return rv;
}

NATIVE_WORD_DECL(keypad, STACK_KEYS) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::KeypadController *kc = dynamic_cast<rpn::KeypadController*>(ctx);
  kc->clearAssignedButtons();

  kc->assignButton(1,2, "DROP");
  kc->assignButton(1,3, "DEPTH");
  kc->assignButton(1,4, "SWAP");
  kc->assignButton(1,5, "ROLLU");
  kc->assignButton(1,6, "ROLLD");
  kc->assignButton(1,7, "OVER");
  kc->assignButton(1,8, "DUP");
  kc->assignButton(1,9, "ROTU");
  kc->assignButton(1,10, "ROTD");

  kc->assignButton(2,2, "DROPn");
  kc->assignButton(2,3, "DUPn");
  kc->assignButton(2,4, "NIPn");
  kc->assignButton(2,5, "PICK");
  kc->assignButton(2,6, "ROLLDn");
  kc->assignButton(2,7, "ROLLUn");
  kc->assignButton(2,8, "TUCKn");
  kc->assignButton(2,9, "REVERSE", "REV");
  kc->assignButton(2,10, "REVERSEn", "REVn");

  return rv;
}

NATIVE_WORD_DECL(keypad, LOGIC_KEYS) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::KeypadController *kc = dynamic_cast<rpn::KeypadController*>(ctx);
  kc->clearAssignedButtons();

  kc->assignButton(1,2, "IFTE");
  kc->assignButton(1,3, "==");
  kc->assignButton(1,4, ">");
  kc->assignButton(1,5, ">=");
  kc->assignButton(1,6, "<");
  kc->assignButton(1,7, "<=");
  kc->assignButton(1,8, "!=");
  //  kc->assignButton(1,9, "");
  //  kc->assignButton(1,10, "");

  kc->assignButton(2,2, "NOT");
  kc->assignButton(2,3, "AND");
  kc->assignButton(2,4, "OR");

  kc->assignButton(2,5, "XOR");

  return rv;
}

NATIVE_WORD_DECL(keypad, TYPE_KEYS) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  rpn::KeypadController *kc = dynamic_cast<rpn::KeypadController*>(ctx);
  kc->clearAssignedButtons();

  kc->assignButton(1,2, "->INT");
  kc->assignButton(1,3, "->FLOAT", "->FLT");
  kc->assignButton(1,4, "->STRING", "->STR");
  kc->assignButton(1,5, "->OBJECT", "->{}");
  kc->assignButton(1,6, "OBJECT->", "{}->");
  kc->assignButton(1,7, "->ARRAY", "->[]");
  kc->assignButton(1,8, "ARRAY->", "[]->");
  //  kc->assignButton(1,9, "->VEC3", "->V3");
  //  kc->assignButton(1,10, "VEC3->", "V3->");

  kc->assignButton(2,2, "->VEC3", "->V3");
  kc->assignButton(2,3, "VEC3->", "V3->");
  kc->assignButton(2,4, "->VEC3x", "->V3x");
  kc->assignButton(2,5, "->VEC3y", "->V3y");
  kc->assignButton(2,6, "->VEC3z", "->V3z");
  /*
  kc->assignButton(2,7, "LN");
  kc->assignButton(2,8, "LN2");
  kc->assignButton(2,9, "LOG");
  kc->assignButton(2,10, "CHS");

  kc->assignButton(3,2,"ROUND");
  kc->assignButton(3,3,"CEIL");
  kc->assignButton(3,4, "FLOOR");
  kc->assignButton(3,5, "k_PI");
  kc->assignButton(3,6, "k_E");
  kc->assignButton(3,7, "RAND");
  kc->assignButton(3,8, "RAND48");
  */
  return rv;
}

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

NATIVE_WORD_DECL(long_word, LOGIC_KEYS) {
  std::this_thread::sleep_for(3s);
  return rpn::WordDefinition::Result::ok;
}

rpn::KeypadController::KeypadController() {
}

void
rpn::KeypadController::add_words(rpn::Interp &rpn) {
  rpn.addDefinition("assign-key", { skAssignValidator, NATIVE_WORD_FN(keypad, ASSIGN_KEY), this });
  rpn.addDefinition("math-keys", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(keypad, MATH_KEYS), this });
  rpn.addDefinition("stack-keys", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(keypad, STACK_KEYS), this });
  rpn.addDefinition("logic-keys", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(keypad, LOGIC_KEYS), this });
  rpn.addDefinition("type-keys", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(keypad, TYPE_KEYS), this });
  rpn.addDefinition("long-word", { rpn::StackSizeValidator::zero, NATIVE_WORD_FN(long_word, LOGIC_KEYS), this });
  clearAssignedButtons();
}

void
rpn::KeypadController::remove_words(rpn::Interp &rpn) {
  rpn.removeDefinition("assign-key");
  rpn.removeDefinition("math-keys");
  rpn.removeDefinition("stack-keys");
  rpn.removeDefinition("logic-keys");
  rpn.removeDefinition("type-keys");
}

// 1 1 ." sqrt" ." SQRT" assign-key

/* end of QInc/Projects/color-calc/src/libs/rpn-lang/src/keypad-dict.cpp */
