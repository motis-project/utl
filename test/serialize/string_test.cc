#include <iostream>

#include "utl/serialization/string.h"

#include "../catch.hpp"

TEST_CASE("string") {
  auto s = utl::string{"yeah", utl::string::owning};
  s.set_owning("hello world");
  CHECK(s.is_short() == true);
  CHECK(s.view() == "hello world");
  s.set_owning("hello world hello world");
  CHECK(s.is_short() == false);
  CHECK(s.view() == "hello world hello world");
}