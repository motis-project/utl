#include "catch2/catch.hpp"

#include "utl/struct/comparable.h"

struct a {
  MAKE_COMPARABLE()
  int i_ = 1;
  int j_ = 2;
  double d_ = 100.0;
  std::string s_ = "hello world";
};

TEST_CASE("comparable") {
  a inst1, inst2;

  CHECK(inst1 == inst2);
  CHECK(!(inst1 != inst2));
  CHECK(inst1 <= inst2);
  CHECK(inst1 >= inst2);
  CHECK(!(inst1 < inst2));
  CHECK(!(inst1 > inst2));

  inst1.j_ = 1;

  CHECK(!(inst1 == inst2));
  CHECK(inst1 != inst2);
  CHECK(inst1 <= inst2);
  CHECK(!(inst1 >= inst2));
  CHECK(inst1 < inst2);
  CHECK(!(inst1 > inst2));
}
