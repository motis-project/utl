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
