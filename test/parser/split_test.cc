#include "utl/parser/split.h"
#include "gtest/gtest.h"

using namespace utl;

TEST(split, simple_split) {
  auto const chunks = utl::split<'_', utl::cstr, utl::cstr, utl::cstr>("a_b_c");
  EXPECT_EQ(cstr{"a"}, std::get<0>(chunks));
  EXPECT_EQ(cstr{"b"}, std::get<1>(chunks));
  EXPECT_EQ(cstr{"c"}, std::get<2>(chunks));
}

TEST(split, too_few_split) {
  auto const chunks = utl::split<'_', utl::cstr, utl::cstr, utl::cstr>("a_b");
  EXPECT_EQ(cstr{"a"}, std::get<0>(chunks));
  EXPECT_EQ(cstr{"b"}, std::get<1>(chunks));
  EXPECT_EQ(cstr{"\0"}, std::get<2>(chunks));
}
