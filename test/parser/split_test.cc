#include <tuple>

#include "gtest/gtest.h"
#include "utl/parser/cstr.h"
#include "utl/parser/split.h"

using namespace utl;

TEST(split, simple_split) {
  auto const chunks = utl::split<'_', utl::cstr, utl::cstr, utl::cstr>("a_b_c");
  EXPECT_EQ(utl::cstr{"a"}, std::get<0>(chunks));
  EXPECT_EQ(utl::cstr{"b"}, std::get<1>(chunks));
  EXPECT_EQ(utl::cstr{"c"}, std::get<2>(chunks));
}

TEST(split, too_few_split) {
  auto const chunks = utl::split<'_', utl::cstr, utl::cstr, utl::cstr>("a_b");
  EXPECT_EQ(utl::cstr{"a"}, std::get<0>(chunks));
  EXPECT_EQ(utl::cstr{"b"}, std::get<1>(chunks));
  EXPECT_EQ(utl::cstr{""}, std::get<2>(chunks));
  EXPECT_FALSE(std::get<2>(chunks).valid());
}

TEST(split, starting_underscore) {
  auto const chunks =
      utl::split<'_', utl::cstr, utl::cstr, utl::cstr>("a_b__c");
  EXPECT_EQ(utl::cstr{"a"}, std::get<0>(chunks));
  EXPECT_EQ(utl::cstr{"b"}, std::get<1>(chunks));
  EXPECT_EQ(utl::cstr{""}, std::get<2>(chunks));
  EXPECT_FALSE(std::get<2>(chunks).valid());
}
