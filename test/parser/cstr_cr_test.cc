#include "../catch.hpp"

#include "utl/parser/cstr.h"

namespace utl {
namespace cstr_cr_test {

const char* one_line = "1\r\n";
const char* one_line_lines[] = {"1"};

const char* one_line_no_end = "1";
const char* one_line_no_end_lines[] = {"1"};

const char* two_lines = "1\r\n2\r\n";
const char* two_lines_lines[] = {"1", "2"};

const char* empty_string = "";

const char* empty_line = "\r\n";
const char* empty_line_lines[] = {""};

const char* empty_line_between = "1\r\n\n2\r\n";
const char* empty_line_between_lines[] = {"1", "", "2"};

const char* empty_line_between_no_end = "1\r\n\n2";
const char* empty_line_between_no_end_lines[] = {"1", "", "2"};

TEST_CASE("get_line_cr") {
  cstr str = one_line;
  auto line = get_line(str);
  REQUIRE(line.len == 1);
  REQUIRE(std::strncmp(line.str, "1", 1) == 0);
  REQUIRE(std::strlen(line.str) == 3);
}

TEST_CASE("one_line_cr") {
  int i = 0;
  for_each_line(one_line, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, one_line_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("one_line_no_end_cr") {
  int i = 0;
  for_each_line(one_line_no_end, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, one_line_no_end_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("two_lines_cr") {
  int i = 0;
  for_each_line(two_lines, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, two_lines_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 2);
}

TEST_CASE("empty_line_cr") {
  int i = 0;
  for_each_line(empty_line, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 1);
}

TEST_CASE("empty_string_cr") {
  int i = 0;
  for_each_line(empty_string, [&](cstr) { i++; });
  REQUIRE(i == 0);
}

TEST_CASE("empty_line_between_cr") {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_between_lines[i++], line.len) ==
            0);
  });
  REQUIRE(i == 3);
}

TEST_CASE("empty_line_between_no_end_cr") {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_between_no_end_lines[i++],
                         line.len) == 0);
  });
  REQUIRE(i == 3);
}

TEST_CASE("substr_len_begin_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(0, size(2)) == "ab");
  REQUIRE(s.substr(0, size(2)).len == 2);
  REQUIRE(s.substr(0, size(2)).str == s.str);
}

TEST_CASE("substr_len_middle_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(1, size(1)) == "b");
  REQUIRE(s.substr(1, size(1)).len == 1);
  REQUIRE(s.substr(1, size(1)).str == s.str + 1);
}

TEST_CASE("sbustr_pos_begin_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(0, 2) == "ab");
  REQUIRE(s.substr(0, 2).len == 2);
  REQUIRE(s.substr(0, 2).str == s.str);
}

TEST_CASE("sbustr_pos_middle_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(1, 2) == "b");
  REQUIRE(s.substr(1, 2).len == 1);
  REQUIRE(s.substr(1, 2).str == s.str + 1);
}

}  // namespace cstr_cr_test
}  // namespace utl
