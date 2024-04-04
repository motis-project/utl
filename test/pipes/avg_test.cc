#include "gtest/gtest.h"

#include <list>

#include "utl/enumerate.h"
#include "utl/pipes.h"
#include "utl/zip.h"

using namespace utl;

TEST(pipes, avg_vec_test) {
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
  EXPECT_TRUE(r0 == r1);
}

TEST(pipes, avg_find_test) {
  std::vector<int> v = {1, 3, 5, 7, 9};
  EXPECT_TRUE(*(all(v) | find([](auto&& i) { return i == 7; })) == 7);
}

TEST(pipes, avg_unique_test) {
  std::vector<int> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  std::vector<int> result = {1, 3, 5, 7, 9};
  EXPECT_TRUE((all(v) | unique() | vec()) == result);

  int i = 0;
  for (auto const val : all(v) | unique() | iterable()) {
    EXPECT_TRUE(val == result[i++]);
  }
}

TEST(pipes, avg_sum_test_default) {
  std::vector<int> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  int result =
      all(v) | unique() | remove_if([](auto&& i) { return i > 3; }) | sum();
  EXPECT_TRUE(result == 4);
}

TEST(pipes, avg_sum_test_template) {
  std::vector<std::size_t> v = {1, 3, 3, 5, 7, 7, 7, 7, 9};
  std::size_t result =
      all(v) | unique() | remove_if([](auto&& i) { return i > 3; }) | sum();
  EXPECT_TRUE(result == std::size_t{4});
}

TEST(pipes, avg_emplace_back) {
  std::vector<int> v = {1, 2, 3, 4};

  auto result = all(v) | transform([](auto&& x) { return x * x; }) |
                emplace_back<std::list<int>>();
  std::list<int> expected = {1, 4, 9, 16};

  EXPECT_TRUE(result == expected);
}

TEST(pipes, avg_insert) {
  std::vector<int> v = {1, 2, 3, 4};

  auto result =
      all(v) |
      transform([](auto&& x) { return std::pair(std::to_string(x), x); }) |
      insert<std::map<std::string, int>>();

  EXPECT_TRUE(result["1"] == 1);
  EXPECT_TRUE(result["2"] == 2);
  EXPECT_TRUE(result["3"] == 3);
  EXPECT_TRUE(result["4"] == 4);
}
