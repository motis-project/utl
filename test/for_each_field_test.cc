#include "catch.hpp"

#include "utl/struct/for_each_field.h"

struct a {
  int i = 1;
  int j = 2;
  double d = 100.0;
  std::string s = "hello world";
};

TEST_CASE("for_each_field") {
  a instance;
  utl::for_each_field(instance, [](auto&& m) { m = {}; });
  CHECK(instance.i == 0);
  CHECK(instance.j == 0);
  CHECK(instance.d == 0.0);
  CHECK(instance.s == "");
}
