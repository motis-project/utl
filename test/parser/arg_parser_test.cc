#include <cmath>

#include "gtest/gtest.h"

#include "utl/parser/arg_parser.h"
#include "utl/parser/csv.h"

using namespace utl;

TEST(arg_parser, parse_int) {
  cstr s = "33.7994;77";
  int i{};
  bool ok = parse_arg(s, i);

  ASSERT_TRUE(!ok);
  ASSERT_TRUE(s.len == 8);
  ASSERT_TRUE(i == 33);
}

TEST(arg_parser, parse_int_str_end) {
  cstr s = "33";
  int i{};
  bool ok = parse_arg(s, i);

  ASSERT_TRUE(ok);
  ASSERT_TRUE(s.len == 0);
  ASSERT_TRUE(i == 33);
}

TEST(arg_parser, parse_negative_int) {
  cstr s = "-33.7994;77";
  int i{};
  parse_arg(s, i);

  ASSERT_TRUE(s.len == 8);
  ASSERT_TRUE(i == -33);
}

TEST(arg_parser, parse_int_default_default_value) {
  cstr s = "a-33.7994;77";
  int i{};
  bool ok = parse_arg(s, i);

  ASSERT_TRUE(!ok);
  ASSERT_TRUE(s.len == 12);
  ASSERT_TRUE(i == 0);
}

TEST(arg_parser, parse_int_different_default_value) {
  cstr s = "a-33.7994;77";
  int i{};
  bool ok = parse_arg(s, i, -1);

  ASSERT_TRUE(!ok);
  ASSERT_TRUE(s.len == 12);
  ASSERT_TRUE(i == -1);
}

TEST(arg_parser, parse_int_different_default_value_sign) {
  cstr s = "-a33.7994;77";
  int i{};
  parse_arg(s, i, -1);

  ASSERT_TRUE(s.len == 11);
  ASSERT_TRUE(i == -1);
}

TEST(arg_parser, parse_string) {
  cstr s = "hello,world";
  std::string string1;
  std::string string2;
  parse_column(s, string1);
  ASSERT_TRUE(s.len == 6);
  ++s;
  ASSERT_TRUE(s.len == 5);
  parse_column(s, string2);

  ASSERT_TRUE(string1 == "hello");
  ASSERT_TRUE(string2 == "world");
}

TEST(arg_parser, parse_string_str_end) {
  cstr s = "hello";
  std::string string;
  parse_column(s, string);

  ASSERT_TRUE(s.len == 0);
  ASSERT_TRUE(string == "hello");
}

TEST(arg_parser, parse_string_quote) {
  cstr s = R"("hello","world")";
  std::string string1;
  std::string string2;
  parse_column(s, string1);
  ASSERT_TRUE(s.len == 8);
  ASSERT_TRUE(*s.str == ',');
  ++s;
  ASSERT_TRUE(s.len == 7);
  parse_column(s, string2);

  ASSERT_TRUE(string1 == "hello");
  ASSERT_TRUE(string2 == "world");
}

TEST(arg_parser, parse_string_str_quote_end) {
  cstr s = "\"hello\"";
  std::string string;
  parse_column(s, string);

  ASSERT_TRUE(s.len == 0);
  ASSERT_TRUE(string == "hello");
}

TEST(arg_parser, parse_string_str_missing_quote_end) {
  cstr s = "\"hello";
  std::string string;
  parse_column(s, string);

  ASSERT_TRUE(s.len == 0);
}

TEST(arg_parser, parse_float) {
  cstr s = "33.7994;77";
  float f{};
  parse_arg(s, f);
  ASSERT_TRUE(s.len == 3);
  ASSERT_TRUE(*s.str == ';');

  float target = 33.7994F;
  ASSERT_TRUE(std::abs(f - target) < 0.001);
}

TEST(arg_parser, parse_negative_float) {
  cstr s = "-33.7894,\"";
  float f{};
  parse_arg(s, f);
  ASSERT_TRUE(s.len == 2);
  ASSERT_TRUE(*s.str == ',');

  float target = -33.7894F;
  ASSERT_TRUE(std::abs(f - target) < 0.001);
}

TEST(arg_parser, parse_float_str_end) {
  cstr s = "33.7994";
  float f{};
  parse_arg(s, f);
  ASSERT_TRUE(s.len == 0);

  float target = 33.7994F;
  ASSERT_TRUE(std::abs(f - target) < 0.001);
}

TEST(arg_parser, parse_negative_float_str_end) {
  cstr s = "-33.7894";
  float f{};
  parse_arg(s, f);
  ASSERT_TRUE(s.len == 0);

  float target = -33.7894F;
  ASSERT_TRUE(std::abs(f - target) < 0.001);
}

TEST(arg_parser, parse_bool_false) {
  cstr s = "false";
  bool b = true;
  parse_arg(s, b);

  ASSERT_TRUE(!b);
}

TEST(arg_parser, parse_bool_true) {
  cstr s = "true";
  bool b = false;
  parse_arg(s, b);

  ASSERT_TRUE(b);
}

TEST(arg_parser, parse_bool_falsy) {
  cstr s = "f..";
  bool b = true;
  parse_arg(s, b);

  ASSERT_TRUE(!b);
  ASSERT_TRUE(s.len == 0);
}

TEST(arg_parser, parse_bool_truey) {
  cstr s = "t..";
  bool b = false;
  auto ok = parse_arg(s, b);

  ASSERT_TRUE(!ok);
  ASSERT_TRUE(b);
  ASSERT_TRUE(s.len == 0);
}

TEST(arg_parser, parse_bool_true_seperator) {
  cstr s = "true,next";
  bool b = false;
  auto ok = parse_arg(s, b);

  ASSERT_TRUE(!ok);
  ASSERT_TRUE(b);
  ASSERT_TRUE(s.len == 5);
}

TEST(arg_parser, parse_bool_false_seperator) {
  cstr s = "false,next";
  bool b = true;
  parse_arg(s, b);

  ASSERT_TRUE(!b);
  ASSERT_TRUE(s.len == 5);
}

TEST(arg_parser, parse_bool_exact_true) {
  bool x = false;
  auto s = cstr{"true"};
  ASSERT_TRUE(parse_arg(s, x));
  ASSERT_TRUE(x == true);
}

TEST(arg_parser, parse_bool_exact_false) {
  bool x = false;
  auto s = cstr{"false"};
  ASSERT_TRUE(parse_arg(s, x));
  ASSERT_TRUE(x == false);
}

TEST(arg_parser, parse_int_substr) {
  cstr s = "123456";
  int a{};
  auto sub = s.substr(2, 4);
  sub >> a;
  ASSERT_TRUE(a == 34);
}
