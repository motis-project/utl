#include "catch2/catch.hpp"

#include "utl/pipes.h"

using namespace utl;

TEST_CASE("flatten") {
  std::vector<std::vector<int>> v = {{1, 2, 3}, {4, 5, 6, 7}};

  auto result = all(v) | flatten() | vec();

  std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7};
  CHECK(expected == result);
}

TEST_CASE("2nd level flatten") {
  std::vector<std::vector<std::vector<int>>> v = {{{1, 2}, {3}},
                                                  {{4, 5}, {6, 7}}};

  auto result = all(v) | flatten() | flatten() | vec();

  std::vector<int> expected = {1, 2, 3, 4, 5, 6, 7};
  CHECK(expected == result);
}

TEST_CASE("flatten before transform") {
  std::vector<std::vector<int>> v = {{1, 2, 3}, {4, 5, 6, 7}};

  auto result =
      all(v) | flatten() | transform([](auto&& i) { return i * i; }) | vec();

  std::vector<int> expected = {1, 4, 9, 16, 25, 36, 49};
  CHECK(expected == result);
}

TEST_CASE("flatten after transform") {
  std::vector<int> v = {1, 2, 3, 4};

  auto result = all(v) | transform([](auto&& i) {
                  return std::vector<int>{i, i};
                }) |
                flatten() | vec();

  std::vector<int> expected = {1, 1, 2, 2, 3, 3, 4, 4};
  CHECK(expected == result);
}

TEST_CASE("flatten after transform struct") {
  struct t {
    std::vector<int> v_;
  };

  std::vector<t> v = {t{.v_ = {1, 2}}, t{.v_ = {42, 77, 1337}}};

  auto result = all(v) |
                transform([](auto&& ta) -> std::vector<int> { return ta.v_; }) |
                flatten() | vec();

  std::vector<int> expected = {1, 2, 42, 77, 1337};
  CHECK(expected == result);
}

TEST_CASE("find after flatten") {
  std::vector<std::vector<int>> v = {{1, 2, 3}, {4, 5, 6, 7}};

  auto result = all(v) | flatten() | find([](auto&& i) { return i == 5; });

  CHECK(result == 5);
}
