#include "gtest/gtest.h"

#include "utl/pipes.h"

using namespace utl;

TEST(pipes, count_int) {
  std::vector<int> v = {1, 3, 5, 7, 9};
  EXPECT_TRUE((all(v) | count([](auto&& i) { return i == 3; })) == 1);
}

TEST(pipes, count_int2) {
  std::vector<int> v = {5, 7, 9, 1, 4, 1, 3, 1, 5, 3, 1};
  EXPECT_TRUE((all(v) | count([](auto&& i) { return i == 1; })) == 4);
}

TEST(pipes, count_int3) {
  std::vector<int> v = {5, 7, 9, 1, 4, 1, 3, 1, 5, 3, 1};
  EXPECT_TRUE((all(v) | count([](auto&& i) { return i == 10; })) == 0);
}
