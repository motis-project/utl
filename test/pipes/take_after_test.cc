#include "catch2/catch.hpp"

#include "utl/pipes.h"

using namespace utl;

TEST_CASE("take after test - take all") {
  std::vector<int> const v = {1, 2, 3, 4, 5, 6, 7};

  auto result = all(v) | take_after([](auto&& i) { return i < 3; }) | vec();

  CHECK(v == result);
}

TEST_CASE("take after test - take some") {
  std::vector<int> const v = {1, 2, 3, 4, 5, 6, 7};

  auto result = all(v) | take_after([](auto&& i) { return i > 3; }) | vec();

  std::vector<int> const expected = {4, 5, 6, 7};
  CHECK(expected == result);
}

TEST_CASE("take after test - take none") {
  std::vector<int> const v = {1, 2, 3, 4, 5, 6, 7};

  auto result = all(v) | take_after([](auto&& i) { return i > 9; }) | vec();

  std::vector<int> const expected = {};
  CHECK(expected == result);
}
