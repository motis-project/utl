#include "catch2/catch.hpp"

#include "utl/pipes.h"

using namespace utl;

TEST_CASE("count test int") {
  std::vector<int> v = {1, 3, 5, 7, 9};
  CHECK((all(v) | count([](auto&& i) { return i == 3; })) == 1);
}

TEST_CASE("count test int 2") {
  std::vector<int> v = {5, 7, 9, 1, 4, 1, 3, 1, 5, 3, 1};
  CHECK((all(v) | count([](auto&& i) { return i == 1; })) == 4);
}

TEST_CASE("count test int 3") {
  std::vector<int> v = {5, 7, 9, 1, 4, 1, 3, 1, 5, 3, 1};
  CHECK((all(v) | count([](auto&& i) { return i == 10; })) == 0);
}
