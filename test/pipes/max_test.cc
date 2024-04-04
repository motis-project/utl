#include "gtest/gtest.h"

#include "utl/pipes.h"

using namespace utl;

TEST(pipes, max_int) {
  std::vector<int> v = {1, 3, 5, 7, 9};
  EXPECT_TRUE((all(v) | max<int>()) == 9);
}

TEST(pipes, max_int_2) {
  std::vector<int> v = {5, 7, 9, 1, 4};
  EXPECT_TRUE((all(v) | max<int>()) == 9);
}

TEST(pipes, max_float) {
  std::vector<double> v = {1.2, 3.5, 5.5, 7.3, 9.8};
  EXPECT_TRUE((all(v) | max<double>()) == 9.8);
}
