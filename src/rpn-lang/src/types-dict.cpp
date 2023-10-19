/***************************************************
 * file: qinc/rpn-lang/src/types-dict.cpp
 *
 * @file    types-dict.cpp
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

#include <cmath>

/***************************************************
 * Integer
 */
// to_integer
NATIVE_WORD_DECL(types, to_int) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}


/***************************************************
 * Float
 */
NATIVE_WORD_DECL(types, to_float) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}

/***************************************************
 * String
 */
NATIVE_WORD_DECL(types, to_string) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}

/***************************************************
 * Object
 */
NATIVE_WORD_DECL(t_object, to_object) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  std::string ident = rpn.stack.pop_string();
  auto val = rpn.stack.pop();
  StObject obj;
  obj.inner().add_value(ident,*val.get());
  rpn.stack.push(obj);
  return rv;
}

NATIVE_WORD_DECL(t_object, object_to) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  return rv;
}

NATIVE_WORD_DECL(t_object, add_string_any_object) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  std::string ident = rpn.stack.pop_string();
  auto val = rpn.stack.pop();
  auto &sob = rpn.stack.peek(1);
  StObject &obj = PEEK_CAST(StObject,sob);
  obj.inner().add_value(ident,*val.get());
  return rv;
}

NATIVE_WORD_DECL(t_object, add_object_string_any) {
  rpn.stack.rolldn(3);
  return NATIVE_WORD_FN(t_object, add_string_any_object)(rpn,ctx,rest);
}

/***************************************************
 * Array
 */
NATIVE_WORD_DECL(t_array, to_array) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  return rv;
}

NATIVE_WORD_DECL(t_array, array_to) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  return rv;
}

NATIVE_WORD_DECL(t_array, add_array_any) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  return rv;
}

NATIVE_WORD_DECL(t_array, add_any_array) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::implementation_error;
  return rv;
}


/***************************************************
 * Vec3
 */

// nan behaves like 0 when added to non-nan
static double nan_add_0(double a, double b) {
  double rv = std::nan("");
  switch((std::isnan(a)<<1)|(std::isnan(b))) {
  case 0:
    rv = a + b;
    break;
  case 1:
    rv = a;
    break;
  case 2:
    rv = b;
    break;
  case 3:
    //    rv is already nan
    break;
  }
  return rv;
}

static double nan_add(double a, double b) {
  double rv = std::nan("");
  switch((std::isnan(a)<<1)|(std::isnan(b))) {
  case 0:
    rv = a + b;
    break;
  case 1:
  case 2:
  case 3:
    //    rv is already nan
    break;
  }
  return rv;
}

static double nan_sub_0(double a, double b) {
  double rv = std::nan("");
  switch((std::isnan(a)<<1)|(std::isnan(b))) {
  case 0:
    rv = a - b;
    break;
  case 1:
    rv = -b;
    break;
  case 2:
    rv = a;
    break;
  case 3:
    // rv is already nan
    break;
  }
  return rv;
}

static double nan_sub(double a, double b) {
  double rv = std::nan("");
  switch((std::isnan(a)<<1)|(std::isnan(b))) {
  case 0:
    rv = a - b;
    break;
  case 1:
  case 2:
  case 3:
    // rv is already nan
    break;
  }
  return rv;
}

// vec3+number
NATIVE_WORD_DECL(vec3, add_vec3) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  auto o1 = rpn.stack.pop();
  auto o2 = rpn.stack.pop();
  const auto &v1 = POP_CAST(StVec3,o1);
  const auto &v2 = POP_CAST(StVec3,o2);
  rpn.stack.push(StVec3(nan_add_0(v1._x, v2._x), nan_add_0(v1._y, v2._y), nan_add_0(v1._z, v2._z)));
  return rv;
}

// number+vec3
NATIVE_WORD_DECL(vec3, add_num_vec3) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double n1 = rpn.stack.pop_as_double();
  auto o2 = rpn.stack.pop();
  const auto &v2 = POP_CAST(StVec3,o2);
  rpn.stack.push(StVec3(nan_add(n1, v2._x), nan_add(n1, v2._y), nan_add(n1, v2._z)));
  return rv;
}

// add two vec3's
NATIVE_WORD_DECL(vec3, add_vec3_num) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  auto o1 = rpn.stack.pop();
  double n2 = rpn.stack.pop_as_double();
  const auto &v1 = POP_CAST(StVec3,o1);
  rpn.stack.push(StVec3(nan_add(v1._x, n2), nan_add(v1._y, n2), nan_add(v1._z, n2)));
  return rv;
}

NATIVE_WORD_DECL(vec3, sub_vec3) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  auto o2 = rpn.stack.pop();
  auto o1 = rpn.stack.pop();
  const auto &v1 = POP_CAST(StVec3,o1);
  const auto &v2 = POP_CAST(StVec3,o2);
  rpn.stack.push(StVec3(nan_sub_0(v1._x, v2._x), nan_sub_0(v1._y, v2._y), nan_sub_0(v1._z, v2._z)));
  return rv;
}

// number+vec3
NATIVE_WORD_DECL(vec3, sub_num_vec3) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double n1 = rpn.stack.pop_as_double();
  auto o2 = rpn.stack.pop();
  const auto &v2 = POP_CAST(StVec3,o2);
  rpn.stack.push(StVec3(nan_sub(n1, v2._x), nan_sub(n1, v2._y), nan_sub(n1, v2._z)));
  return rv;
}

// add two vec3's
NATIVE_WORD_DECL(vec3, sub_vec3_num) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  auto o1 = rpn.stack.pop();
  double n2 = rpn.stack.pop_as_double();
  const auto &v1 = POP_CAST(StVec3,o1);
  rpn.stack.push(StVec3(nan_sub(v1._x, n2), nan_sub(v1._y, n2), nan_sub(v1._z, n2)));
  return rv;
}

// 3 numbers to a vec3
NATIVE_WORD_DECL(vec3, to_vec3) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double z = rpn.stack.pop_as_double();
  double y = rpn.stack.pop_as_double();
  double x = rpn.stack.pop_as_double();
  rpn.stack.push(StVec3(x,y,z));
  return rv;
}

// vec3 to 3 doubles
NATIVE_WORD_DECL(vec3, vec3_to) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  auto o1 = rpn.stack.pop();
  const auto &v1 = POP_CAST(StVec3,o1);
  rpn.stack.push_double(v1._x);
  rpn.stack.push_double(v1._y);
  rpn.stack.push_double(v1._z);
  return rv;
}

//  number to x-value in Vec3
NATIVE_WORD_DECL(vec3, to_vec3x) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double n1 = rpn.stack.pop_as_double();
  rpn.stack.push(StVec3(n1,std::nan(""),std::nan("")));
  return rv;
}

//  number to y-value in Vec3
NATIVE_WORD_DECL(vec3, to_vec3y) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double n1 = rpn.stack.pop_as_double();
  rpn.stack.push(StVec3(std::nan(""),n1,std::nan("")));
  return rv;
}

//  number to z-value in Vec3
NATIVE_WORD_DECL(vec3, to_vec3z) {
  rpn::WordDefinition::Result rv = rpn::WordDefinition::Result::ok;
  double n1 = rpn.stack.pop_as_double();
  rpn.stack.push(StVec3(std::nan(""),std::nan(""),n1));
  return rv;
}

void
rpn::Interp::addTypeWords() {
  addDefinition("->INT", NATIVE_WORD_WDEF(types, rpn::StackSizeValidator::one, to_int, nullptr));
  addDefinition("->FLOAT", NATIVE_WORD_WDEF(types, rpn::StackSizeValidator::one, to_float, nullptr));
  addDefinition("->STRING", NATIVE_WORD_WDEF(types, rpn::StackSizeValidator::one, to_string, nullptr));

  addDefinition("->OBJECT", NATIVE_WORD_WDEF(t_object, rpn::StrictTypeValidator::d2_string_any, to_object, nullptr));
  addDefinition("OBJECT->", NATIVE_WORD_WDEF(t_object, rpn::StrictTypeValidator::d1_object, object_to, nullptr));
  addDefinition("OBJ->", NATIVE_WORD_WDEF(t_object, rpn::StrictTypeValidator::d1_object, object_to, nullptr));
  addDefinition("->ARRAY", NATIVE_WORD_WDEF(t_array, rpn::StackSizeValidator::ntos, to_array, nullptr));
  addDefinition("ARRAY->", NATIVE_WORD_WDEF(t_array, rpn::StrictTypeValidator::d1_array, array_to, nullptr));
  addDefinition("OBJ->", NATIVE_WORD_WDEF(t_array, rpn::StrictTypeValidator::d1_array, array_to, nullptr));

  addDefinition("+", NATIVE_WORD_WDEF(t_object, rpn::StrictTypeValidator::d3_object_string_any, add_object_string_any, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(t_object, rpn::StrictTypeValidator::d3_string_any_object, add_string_any_object, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(t_array, rpn::StrictTypeValidator::d2_array_any, add_array_any, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(t_array, rpn::StrictTypeValidator::d2_any_array, add_any_array, nullptr));

  addDefinition("+", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_vec3, add_vec3, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_double, add_vec3_num, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_integer, add_vec3_num, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_double_vec3, add_num_vec3, nullptr));
  addDefinition("+", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_integer_vec3, add_num_vec3, nullptr));

  addDefinition("-", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_vec3, sub_vec3, nullptr));
  addDefinition("-", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_double, sub_vec3_num, nullptr));
  addDefinition("-", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_vec3_integer, sub_vec3_num, nullptr));
  addDefinition("-", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_double_vec3, sub_num_vec3, nullptr));
  addDefinition("-", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d2_integer_vec3, sub_num_vec3, nullptr));

  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_double_double_double, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_integer_integer_integer, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_integer_double_double, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_double_integer_double, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_double_double_integer, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_double_integer_integer, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_integer_double_integer, to_vec3, nullptr));
  addDefinition("->VEC3", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d3_integer_integer_double, to_vec3, nullptr));

  addDefinition("->VEC3x", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_double, to_vec3x, nullptr));
  addDefinition("->{x}", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_integer, to_vec3x, nullptr));
  addDefinition("->VEC3y", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_double, to_vec3y, nullptr));
  addDefinition("->{y}", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_integer, to_vec3y, nullptr));
  addDefinition("->VEC3z", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_double, to_vec3z, nullptr));
  addDefinition("->{z}", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_integer, to_vec3z, nullptr));
  addDefinition("VEC3->", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_vec3, vec3_to, nullptr));
  addDefinition("OBJ->", NATIVE_WORD_WDEF(vec3, rpn::StrictTypeValidator::d1_vec3, vec3_to, nullptr));

  //  std:: string line = ": VEC3->{xy} ( <v3> <v3'> ) VEC3-> DROP ->{y} SWAP ->{x} + ;";
  //  line = ": VEC3->{xy} ( <v3> -- <v3'> ) VEC3-> DROP ->{y} SWAP ->{x} + ;";
  //  auto st = parse(line);
  //  line = ": ->{xy} ( x y --  <v3'> ) ->{x} SWAP ->{y} + ;";
  //   st = parse(line);

  auto st = sync_eval(": VEC3->{xy} ( <v3> <v3'> ) VEC3-> DROP ->{y} SWAP ->{x} + ;");
  st = sync_eval(": ->{xy} ( x y --  <v3'> ) ->{x} SWAP ->{y} + ;");
}

/* end of qinc/rpn-lang/src/types-dict.cpp */
