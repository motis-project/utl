#include "catch2/catch.hpp"

#include "utl/parser/cstr.h"

namespace utl::cstr_test {

constexpr char const* one_line = "1\n";
constexpr char const* one_line_lines[] = {"1"};

constexpr char const* one_line_no_end = "1";
constexpr char const* one_line_no_end_lines[] = {"1"};

constexpr char const* two_lines = "1\n2\n";
constexpr char const* two_lines_lines[] = {"1", "2"};

constexpr char const* empty_string = "";

constexpr char const* empty_line = "\n";
constexpr char const* empty_line_lines[] = {""};

constexpr char const* empty_line_between = "1\n\n2\n";
constexpr char const* empty_line_between_lines[] = {"1", "", "2"};

constexpr char const* empty_line_between_no_end = "1\n\n2";
constexpr char const* empty_line_between_no_end_lines[] = {"1", "", "2"};

TEST_CASE("get_line") {
  cstr str = one_line;
  auto line = get_line(str);
  REQUIRE(line.len == 1);
  REQUIRE(std::strncmp(line.str, "1", 1) == 0);
  REQUIRE(std::strlen(line.str) == 2);
}

TEST_CASE("one_line") {
  int i = 0;
  for_each_line(one_line, [&](cstr line) {
    REQUIRE(i < 2);
    REQUIRE(std::strncmp(line.str, one_line_lines[i++], line.len) ==  // NOLINT
            0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("one_line_no_end") {
  int i = 0;
  for_each_line(one_line_no_end, [&](cstr line) {
    REQUIRE(
        std::strncmp(line.str, one_line_no_end_lines[i++], line.len)  // NOLINT
        == 0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("two_lines") {
  int i = 0;
  for_each_line(two_lines, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, two_lines_lines[i++], line.len)  // NOLINT
            == 0);
  });
  REQUIRE(i == 2);
}

TEST_CASE("empty_line") {
  int i = 0;
  for_each_line(empty_line, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_lines[i++], line.len)  // NOLINT
            == 0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("empty_string") {
  int i = 0;
  for_each_line(empty_string, [&](cstr) { ++i; });
  REQUIRE(i == 0);
}

TEST_CASE("empty_line_between") {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_between_lines[i++],  // NOLINT
                         line.len) == 0);
  });
  REQUIRE(i == 3);
}

TEST_CASE("empty_line_between_no_end") {
  int i = 0;
  for_each_line(empty_line_between_no_end, [&](cstr line) {
    REQUIRE(std::strncmp(line.str,  // NOLINT
                         empty_line_between_no_end_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 3);
}

TEST_CASE("substr_len_begin") {
  cstr s = "abc";
  REQUIRE(s.substr(0, size(2)) == "ab");
  REQUIRE(s.substr(0, size(2)).len == 2);
  REQUIRE(s.substr(0, size(2)).str == s.str);
}

TEST_CASE("substr_len_middle") {
  cstr s = "abc";
  REQUIRE(s.substr(1, size(1)) == "b");
  REQUIRE(s.substr(1, size(1)).len == 1);
  REQUIRE(s.substr(1, size(1)).str == s.str + 1);
}

TEST_CASE("sbustr_pos_begin") {
  cstr s = "abc";
  REQUIRE(s.substr(0, 2) == "ab");
  REQUIRE(s.substr(0, 2).len == 2);
  REQUIRE(s.substr(0, 2).str == s.str);
}

TEST_CASE("sbustr_pos_middle") {
  cstr s = "abc";
  REQUIRE(s.substr(1, 2) == "b");
  REQUIRE(s.substr(1, 2).len == 1);
  REQUIRE(s.substr(1, 2).str == s.str + 1);
}

}  // namespace utl::cstr_test
