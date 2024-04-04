#include "gtest/gtest.h"

#include "utl/is_uniform.h"

TEST(is_uniform, is_uniform) {
  using utl::is_uniform;
  std::vector<int> v1{1, 1, 3};
  std::vector<int> v2{1, 1, 1};
  EXPECT_TRUE(!is_uniform(v1));
  EXPECT_TRUE(is_uniform(v2));
}
