#include "catch2/catch_all.hpp"

#include "utl/is_uniform.h"

TEST_CASE("is_uniform") {
  using utl::is_uniform;
  std::vector<int> v1{1, 1, 3};
  std::vector<int> v2{1, 1, 1};
  CHECK(!is_uniform(v1));
  CHECK(is_uniform(v2));
}
