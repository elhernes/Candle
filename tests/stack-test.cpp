/***************************************************
 * file: qinc/rpn-lang/tests/test-stack.cpp
 *
 * @file    test-stack.cpp
 * @author  Eric L. Hernes
 * @version V1.0
 * @born_on   Friday, May 26, 2023
 * @copyright (C) Copyright Eric L. Hernes 2023
 * @copyright (C) Copyright Q, Inc. 2023
 *
 * @brief   An Eric L. Hernes Signature Series C++ module
 *
 */

#include <catch2/catch_test_macros.hpp>
#include "rpn.h"

class CustomArray : public rpn::Stack::Object {
public:
  CustomArray() = default;
  CustomArray(const CustomArray &a)  {
    for(auto const &e : a._v) {
      _v.push_back(e->deep_copy());
    }
  }
  virtual bool operator==(const rpn::Stack::Object &orhs) const override {
    auto &rhs = PEEK_CAST(const CustomArray,orhs);
    bool rv = _v.size() == rhs._v.size();
    for(auto i=_v.cbegin(), j=rhs._v.cbegin(); rv && i!= _v.cend(); i++,j++) {
      rv &= (**i == **j);
    }
    return rv;
  }
  virtual std::unique_ptr<rpn::Stack::Object> deep_copy() const override { return std::make_unique<CustomArray>(*this); };
  void add_value(const rpn::Stack::Object &val) {
    _v.push_back(val.deep_copy());
  }
  virtual operator std::string() const override {
    std::string rv = "[";
    for(auto const &e : _v) {
      rv += e->to_string();
      rv += ", ";
    }
    rv += "]";
    return rv;
  };
  auto const &val() const { return _v; };
private:
  std::vector<std::unique_ptr<rpn::Stack::Object>> _v;
};

class CustomObject {
 public:
  CustomObject(const std::string &v) : _v(v) {};
  virtual operator std::string() const { return std::string("CustomObject: <") + _v + ">"; };
  auto val() const { return _v; }
  bool operator==(const CustomObject &rhs) const {
    return _v == rhs._v;
  }
  bool operator>(const CustomObject &rhs) const {
    return _v > rhs._v;
  }
  bool operator<(const CustomObject &rhs) const {
    return _v < rhs._v;
  }
private:
  std::string _v;
};

using StCustomObject = TStackObject<CustomObject>;

static int stackCount = 0;
static rpn::Stack stack;

TEST_CASE("peek and types" "stack") {
  stack.push_double(3.14159265359); stackCount++;
  REQUIRE(stack.depth() == 1);

  stack.push_double(2.71828182846); stackCount++;
  stack.push_string("abcdefg"); stackCount++;
  stack.push_integer(1023); stackCount++;
  stack.push_boolean(true); stackCount++;
  REQUIRE(stack.depth() == stackCount);

  /*
   * now we have the stack as:
   *
   * 5: pi
   * 4: e
   * 3: "abcdefg"
   * 2: 1023
   * 1: true
   */
  REQUIRE( true == stack.peek_boolean(1) );
  REQUIRE( 1023 == stack.peek_integer(2) );
  REQUIRE( "abcdefg" == stack.peek_string(3) );
  REQUIRE( 2.71828182846 == stack.peek_double(4) );
  REQUIRE( 3.14159265359 == stack.peek_double(5) );
  REQUIRE( "1023" == stack.peek_as_string(2) ); // converts to string
  REQUIRE( 1023.0 == stack.peek_as_double(2) ); // converts to double

  CHECK_THROWS(stack.peek_string(2) == "1023"); // cast fails

  {
    REQUIRE( stack.peek_double(5) ==  3.14159265359);
  }

  {
    const std::string s1 = "string1";
    stack.push_string(s1);
    auto s2 = stack.pop_string();
    REQUIRE(s2 == s1);
  }
}

TEST_CASE("stack operations" "stack") {
  stack.push_integer(511);
  stack.clear();
  stackCount = 0;
  REQUIRE(stack.depth() == stackCount);

  stack.push_double(3.14159265359); stackCount++;
  stack.push_double(2.71828182846); stackCount++;
  stack.push_string("abcdefg"); stackCount++;
  stack.push_integer(1023); stackCount++;
  stack.push_boolean(true); stackCount++;
  REQUIRE(stack.depth() == stackCount);
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // push / pop
  stack.push_boolean(false);
  REQUIRE( stack.pop_boolean() == false );
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // drop
  stack.push_integer(123);
  stack.drop();
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // dropn
  stack.push_integer(123);
  stack.push_integer(123);
  stack.push_integer(123);
  stack.push_integer(123);
  stack.dropn(4);
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( stack.peek_boolean(1) == true );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // dup
  stack.dup();
  REQUIRE( stack.depth() == stackCount+1 );
  REQUIRE( stack.peek_boolean(1) == true );
  REQUIRE( stack.peek_boolean(2) == true );
  stack.drop();
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // dupn
  stack.dupn(3);
  REQUIRE( stack.depth() == stackCount+3 );
  REQUIRE( (stack.peek_boolean(1) == true && stack.peek_boolean(4)==true) );
  REQUIRE( (stack.peek_integer(2) == 1023 && stack.peek_integer(5) == 1023) );
  REQUIRE( (stack.peek_string(3) == "abcdefg" && stack.peek_string(6) == "abcdefg") );
  stack.dropn(3);
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // swap
  REQUIRE( (stack.peek_boolean(1) == true && stack.peek_integer(2)==1023) );
  stack.swap();
  REQUIRE( (stack.peek_boolean(2) == true && stack.peek_integer(1)==1023) );
  stack.swap();
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // nip  / nipn
  stack.dupn(2);
  stack.nipn(2); // drops the second '1023'
  REQUIRE( (stack.peek_boolean(1) == true && stack.peek_boolean(2)==true) );
  stack.drop();
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // over
  stack.over(); stackCount++;
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( (stack.peek_integer(1) == 1023 && stack.peek_boolean(2)==true && stack.peek_integer(3) == 1023) );
  stack.drop(); stackCount--;
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // pick
  stack.pick(3);
  REQUIRE( stack.depth() == stackCount+1 );
  REQUIRE( (stack.peek_string(1) == "abcdefg" && stack.peek_boolean(2)==true && stack.peek_integer(3) == 1023) );
  stack.drop();
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  // rolld
  stack.rolld();
  REQUIRE( 3.14159265359 == stack.peek_double(4) );
  stack.rolld();
  REQUIRE( 3.14159265359 == stack.peek_double(3) );
  stack.rolld();
  REQUIRE( 3.14159265359 == stack.peek_double(2) );
  stack.rolld();
  REQUIRE( 3.14159265359 == stack.peek_double(1) );
  stack.rolld();
  REQUIRE( 3.14159265359 == stack.peek_double(5) );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // rollu();
  stack.rollu();
  REQUIRE( 3.14159265359 == stack.peek_double(1) );
  stack.rollu();
  REQUIRE( 3.14159265359 == stack.peek_double(2) );
  stack.rollu();
  REQUIRE( 3.14159265359 == stack.peek_double(3) );
  stack.rollu();
  REQUIRE( 3.14159265359 == stack.peek_double(4) );
  stack.rollu();
  REQUIRE( 3.14159265359 == stack.peek_double(5) );
  REQUIRE( ((3.14159265359 == stack.peek_double(5)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (1023 == stack.peek_integer(2)) &&
	    (true == stack.peek_boolean(1))) );

  // tuckn
  stack.tuckn(4); stackCount++;
  REQUIRE( true == stack.peek_boolean(4) );
  stack.rollun(4);
  stack.drop(); stackCount--;
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  // rotu
  stack.rotu();
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((true == stack.peek_boolean(2)) &&
	    (1023 == stack.peek_integer(3)) &&
	    ("abcdefg" == stack.peek_string(1)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))));
  stack.rotu();
  stack.rotu();
  // back to original
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  // rotd
  stack.rotd();
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((true == stack.peek_boolean(3)) &&
	    (1023 == stack.peek_integer(1)) &&
	    ("abcdefg" == stack.peek_string(2)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))));
  stack.rotd();
  stack.rotd();
  // back to original
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  // rollun
  stack.rollun(4);
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((true == stack.peek_boolean(2)) &&
	    (1023 == stack.peek_integer(3)) &&
	    ("abcdefg" == stack.peek_string(4)) &&
	    (2.71828182846 == stack.peek_double(1)) &&
	    (3.14159265359 == stack.peek_double(5))));
  stack.rollun(4);
  stack.rollun(4);
  stack.rollun(4);
  // back to original
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  // rotd
  stack.rolldn(4);
  REQUIRE( stack.depth() == stackCount );
  REQUIRE( ((true == stack.peek_boolean(4)) &&
	    (1023 == stack.peek_integer(1)) &&
	    ("abcdefg" == stack.peek_string(2)) &&
	    (2.71828182846 == stack.peek_double(3)) &&
	    (3.14159265359 == stack.peek_double(5))));
  stack.rolldn(4);
  stack.rolldn(4);
  stack.rolldn(4);
  // back to original
  REQUIRE( ((true == stack.peek_boolean(1)) &&
	    (1023 == stack.peek_integer(2)) &&
	    ("abcdefg" == stack.peek_string(3)) &&
	    (2.71828182846 == stack.peek_double(4)) &&
	    (3.14159265359 == stack.peek_double(5))) );

  stack.push_string("wxyz"); stackCount++;
  REQUIRE (stack.depth() == stackCount);
  REQUIRE("wxyz" == stack.peek_string(1));

  stack.push_integer(1234); stackCount++;
  REQUIRE (stack.depth() == stackCount);
  stack.swap();
  try {
    REQUIRE(1234 == stack.peek_integer(2));
  } catch (std::bad_cast e) {
    printf("1234 caught: %s\n", e.what());
  }

  REQUIRE(2.787 != stack.peek_double(stackCount));
  CHECK_THROWS(stack.peek_integer(stackCount) == 0); // conversion fails
  
  {
    CustomArray ar1;
    ar1.add_value(StDouble(9.8));
    ar1.add_value(StString(std::string("yyz")));
    stack.push(ar1); stackCount++;
    REQUIRE (stack.depth() == stackCount);
    auto &ar2 = PEEK_CAST(CustomArray,stack.peek(1));
    printf("ar1: %s\n", std::string(ar1).c_str());
    printf("ar2: %s\n", std::string(ar2).c_str());
    REQUIRE(ar1 == ar2);

    ar1.add_value(ar2);
    REQUIRE(!(ar1 == ar2));
  }

}

// TEST_CASE("object-test StDouble", "[single-file]") {}
// TEST_CASE("object-test StInteger", "[single-file]") {}
// TEST_CASE("object-test StString", "[single-file]") {}
// TEST_CASE("object-test StBoolean", "[single-file]") {}
// TEST_CASE("object-test StObject", "[single-file]") {}
// TEST_CASE("object-test StArray", "[single-file]") {}
// TEST_CASE("object-test CustomArray", "[single-file]") {}
// TEST_CASE("object-test CustomObject", "[single-file]") {}
// TEST_CASE("object-test Inheritance", "[single-file]") {}


int
rpn_main(int ac, char **av) {

  //  stack.push(StDouble(3.1415));
  stack.push_double(3.1415); stackCount++;
  assert (stack.depth() == stackCount);
  stack.push_string("wxyz"); stackCount++;
  assert (stack.depth() == stackCount);
  //  stack.push(StInteger(1234));
  stack.push_integer(1234); stackCount++;
  assert (stack.depth() == stackCount);
  {
    CustomArray ar1;
    ar1.add_value(StDouble(9.8));
    ar1.add_value(StString(std::string("yyz")));
    stack.push(ar1); stackCount++;
    assert (stack.depth() == stackCount);
  }
		 
  stack.push(StString(std::string("abcdefg"))); stackCount++;
  assert (stack.depth() == stackCount);

  stack.push(StDouble(19.2)); stackCount++;
  assert (stack.depth() == stackCount);

  assert (stack.depth() == stackCount);

  {
    double v = stack.pop_double(); stackCount--;// std::unique_ptr<StDouble> v(static_cast<StDouble*>(stack.pop().release()));
    printf("popped double (%f)\n", v);
    assert (stack.depth() == stackCount);
}

  {
    printf("peeked [%s]\n", stack.peek_string(1).c_str());
    stack.print();
  }

  stack.push(StCustomObject(std::string("yyz"))); stackCount++;
  assert (stack.depth() == stackCount);
  
  stack.push_boolean(true); stackCount++;

  stack.swap();
  stack.print("swap");

  stack.rolldn(2);
  stack.print("rolld(2) [aka swap]");
  
  stack.pick(2); stackCount++;
  stack.print("over");

  stack.pick(4); stackCount++;
  stack.print("pick(4)");

  stack.push(StDouble(2.7)); stackCount++;
  stack.dup(); stackCount++;
  assert (stack.depth() == stackCount);
  stack.print("push(2.7) dup");

  //  stack.push(StString("marker"));
  int n = stack.depth();
  stack.dupn(n);  stackCount+=n;
  assert (stack.depth() == stackCount);
  stack.print("depth dupn");

  stack.dropn(n); stackCount-=n;
  assert (stack.depth() == stackCount);
  stack.print("dropn");

  stack.dupn(n); stackCount+=n;
  assert (stack.depth() == stackCount);
  stack.print("dupn");

  stack.rollun(n/2);
  assert (stack.depth() == stackCount);
  printf("rollun(%d)\n", n/2);
  stack.print("rollun");
  assert (stack.depth() == stackCount);

  stack.rollu();
  assert (stack.depth() == stackCount);
  stack.rollu();
  assert (stack.depth() == stackCount);
  stack.print("rollu rollu");

  stack.rolld();
  assert (stack.depth() == stackCount);
  stack.print("rolld");

  stack.rolldn(4);
  assert (stack.depth() == stackCount);
  stack.print("rolld(4)");

  stack.tuckn(5); stackCount++;
  assert (stack.depth() == stackCount);
  stack.print("tuckn(5)");

  stack.nipn(7); stackCount--;
  assert (stack.depth() == stackCount);
  stack.print("nipn(7)");

  StObject obj;
  n=stack.depth();
  for(unsigned i=0; i<n/2; i++) {
    std::string key="v"+std::to_string(i);
    obj.inner().add_value(key, *stack.pop());
  }
  stack.push(obj);
  stack.rolld();
  stack.print("object");

  StArray arr;
  n=stack.depth();
  for(unsigned i=0; i<(n-1); i++) {
    arr.inner().add_value(*stack.pop());
  }
  stack.push(arr);
  stack.print("array");

  return 0;
  //  StDouble pi(3.1415); // std::make_unique<double>(3.14159));
  //  s_stack.push_back(TStackTypePointer<StackType>(pi));
  //  s_stack.push_back(new TStackType<double>(9.876e23));
  //  s_stack.push_back(new TStackType<int>(23));
  //  auto v = s_stack.back();

  
}

/* end of qinc/rpn-lang/tests/test-stack.cpp */
