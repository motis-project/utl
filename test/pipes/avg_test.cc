#include "catch2/catch.hpp"

#include <list>

#include "utl/enumerate.h"
#include "utl/pipes.h"
#include "utl/zip.h"

using namespace utl;

TEST_CASE("vec test") {
  std::list<int> v = {1, 2, 3, 4};
  auto r0 = all(v)  //
            | transform([](auto&& i) { return i * i; })  //
            | remove_if([](auto&& i) { return i > 3; })  //
            | vec();
  auto r1 =
      generate([i = 0]() mutable { return static_cast<void>(--i), -i * i; })  //
      | take_while([](auto&& i) { return i > -5; })  //
      | transform([](auto&& i) { return std::abs(i); })  //
      | remove_if([](auto&& i) { return i > 3; })  //
      | unique()  //
      | vec();
  CHECK(r0 == r1);
}

TEST_CASE("find test") {
  std::vector<int> v = {1, 3, 5, 7, 9};
  CHECK(*(all(v) | find([](auto&& i) { return i == 7; })) == 7);
}

TEST_CASE("unique test") {
  std::vector<int> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  std::vector<int> result = {1, 3, 5, 7, 9};
  CHECK((all(v) | unique() | vec()) == result);

  int i = 0;
  for (auto const val : all(v) | unique() | iterable()) {
    CHECK(val == result[i++]);
  }
}

TEST_CASE("sum test default") {
  std::vector<int> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  int result =
      all(v) | unique() | remove_if([](auto&& i) { return i > 3; }) | sum();
  CHECK(result == 4);
}

TEST_CASE("sum test template") {
  std::vector<std::size_t> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  std::size_t result = all(v) | unique() |
                       remove_if([](auto&& i) { return i > 3; }) |
                       sum<std::size_t>();
  CHECK(result == std::size_t{4});
}

TEST_CASE("emplace back") {
  std::vector<int> v = {1, 2, 3, 4};

  auto result = all(v) | transform([](auto&& x) { return x * x; }) |
                emplace_back<std::list<int>>();
  std::list<int> expected = {1, 4, 9, 16};

  CHECK(result == expected);
}
