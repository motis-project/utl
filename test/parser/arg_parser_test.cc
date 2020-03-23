#include <cmath>

#include "catch/catch.hpp"

#include "utl/parser/arg_parser.h"
#include "utl/parser/csv.h"

using namespace utl;

TEST_CASE("parse_int") {
  cstr s = "33.7994;77";
  int i;
  parse_arg(s, i);

  REQUIRE(s.len == 8);
  REQUIRE(i == 33);
}

TEST_CASE("parse_int_str_end") {
  cstr s = "33";
  int i;
  parse_arg(s, i);

  REQUIRE(s.len == 0);
  REQUIRE(i == 33);
}

TEST_CASE("parse_negative_int") {
  cstr s = "-33.7994;77";
  int i;
  parse_arg(s, i);

  REQUIRE(s.len == 8);
  REQUIRE(i == -33);
}

TEST_CASE("parse_int_default_default_value") {
  cstr s = "a-33.7994;77";
  int i;
  parse_arg(s, i);

  REQUIRE(s.len == 12);
  REQUIRE(i == 0);
}

TEST_CASE("parse_int_different_default_value") {
  cstr s = "a-33.7994;77";
  int i;
  parse_arg(s, i, -1);

  REQUIRE(s.len == 12);
  REQUIRE(i == -1);
}

TEST_CASE("parse_int_different_default_value_sign") {
  cstr s = "-a33.7994;77";
  int i;
  parse_arg(s, i, -1);

  REQUIRE(s.len == 11);
  REQUIRE(i == -1);
}

TEST_CASE("parse_string") {
  cstr s = "hello,world";
  std::string string1;
  std::string string2;
  parse_column(s, string1);
  REQUIRE(s.len == 6);
  ++s;
  REQUIRE(s.len == 5);
  parse_column(s, string2);

  REQUIRE(string1 == "hello");
  REQUIRE(string2 == "world");
}

TEST_CASE("parse_string_str_end") {
  cstr s = "hello";
  std::string string;
  parse_column(s, string);

  REQUIRE(s.len == 0);
  REQUIRE(string == "hello");
}

TEST_CASE("parse_string_\"") {
  cstr s = R"("hello","world")";
  std::string string1;
  std::string string2;
  parse_column(s, string1);
  REQUIRE(s.len == 8);
  REQUIRE(*s.str == ',');
  ++s;
  REQUIRE(s.len == 7);
  parse_column(s, string2);

  REQUIRE(string1 == "hello");
  REQUIRE(string2 == "world");
}

TEST_CASE("parse_string_str_\"_end") {
  cstr s = "\"hello\"";
  std::string string;
  parse_column(s, string);

  REQUIRE(s.len == 0);
  REQUIRE(string == "hello");
}

TEST_CASE("parse_string_str_missing_\"_end") {
  cstr s = "\"hello";
  std::string string;
  parse_column(s, string);

  REQUIRE(s.len == 0);
}

TEST_CASE("parse_float") {
  cstr s = "33.7994;77";
  float f;
  parse_arg(s, f);
  REQUIRE(s.len == 3);
  REQUIRE(*s.str == ';');

  float target = 33.7994F;
  REQUIRE(std::abs(f - target) < 0.001);
}

TEST_CASE("parse_negative_float") {
  cstr s = "-33.7894,\"";
  float f;
  parse_arg(s, f);
  REQUIRE(s.len == 2);
  REQUIRE(*s.str == ',');

  float target = -33.7894F;
  REQUIRE(std::abs(f - target) < 0.001);
}

TEST_CASE("parse_float_str_end") {
  cstr s = "33.7994";
  float f;
  parse_arg(s, f);
  REQUIRE(s.len == 0);

  float target = 33.7994F;
  REQUIRE(std::abs(f - target) < 0.001);
}

TEST_CASE("parse_negative_float_str_end") {
  cstr s = "-33.7894";
  float f;
  parse_arg(s, f);
  REQUIRE(s.len == 0);

  float target = -33.7894F;
  REQUIRE(std::abs(f - target) < 0.001);
}

TEST_CASE("parse_bool_false") {
  cstr s = "false";
  bool b = true;
  parse_arg(s, b);

  REQUIRE(!b);
}

TEST_CASE("parse_bool_true") {
  cstr s = "true";
  bool b = false;
  parse_arg(s, b);

  REQUIRE(b);
}

TEST_CASE("parse_bool_falsy") {
  cstr s = "f..";
  bool b = true;
  parse_arg(s, b);

  REQUIRE(!b);
  REQUIRE(s.len == 0);
}

TEST_CASE("parse_bool_truey") {
  cstr s = "t..";
  bool b = false;
  parse_arg(s, b);

  REQUIRE(b);
  REQUIRE(s.len == 0);
}

TEST_CASE("parse_bool_true_seperator") {
  cstr s = "true,next";
  bool b = false;
  parse_arg(s, b);

  REQUIRE(b);
  REQUIRE(s.len == 5);
}

TEST_CASE("parse_bool_false_seperator") {
  cstr s = "false,next";
  bool b = true;
  parse_arg(s, b);

  REQUIRE(!b);
  REQUIRE(s.len == 5);
}

TEST_CASE("parse_int_substr") {
  cstr s = "123456";
  int a;
  auto sub = s.substr(2, 4);
  sub >> a;
  REQUIRE(a == 34);
}
