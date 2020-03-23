#include "catch/catch.hpp"
#include "utl/enumerate.h"
#include "utl/parser/cstr.h"

namespace utl {
namespace cstr_cr_test {

constexpr char const* one_line = "1\r\n";
constexpr char const* one_line_lines[] = {"1"};

constexpr char const* one_line_no_end = "1";
constexpr char const* one_line_no_end_lines[] = {"1"};

constexpr char const* two_lines = "1\r\n2\r\n";
constexpr char const* two_lines_lines[] = {"1", "2"};

constexpr char const* empty_string = "";

constexpr char const* empty_line = "\r\n";
constexpr char const* empty_line_lines[] = {""};

constexpr char const* empty_line_between = "1\r\n\n2\r\n";
constexpr char const* empty_line_between_lines[] = {"1", "", "2"};

constexpr char const* empty_line_between_no_end = "1\r\n\n2";
constexpr char const* empty_line_between_no_end_lines[] = {"1", "", "2"};

TEST_CASE("line iterator test") {
  line_iterator it{empty_line_between_no_end};
  line_iterator end;

  REQUIRE(it != end);
  REQUIRE(*it == empty_line_between_no_end_lines[0]);
  ++it;

  REQUIRE(it != end);
  REQUIRE(*it == empty_line_between_no_end_lines[1]);
  ++it;

  REQUIRE(it != end);
  REQUIRE(*it == empty_line_between_no_end_lines[2]);
  ++it;

  REQUIRE(it == end);
}

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

  i = 0;
  for (auto const [idx, line] : enumerate(lines{one_line})) {
    REQUIRE(std::strncmp(line.str, one_line_lines[idx], line.len) == 0);
    ++i;
  }
  REQUIRE(i == 1);
}

TEST_CASE("one_line_no_end_cr") {
  int i = 0;
  for_each_line(one_line_no_end, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, one_line_no_end_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 1);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{one_line_no_end})) {
    REQUIRE(std::strncmp(line.str, one_line_no_end_lines[idx], line.len) == 0);
    ++i;
  }
  REQUIRE(i == 1);
}

TEST_CASE("two_lines_cr") {
  int i = 0;
  for_each_line(two_lines, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, two_lines_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 2);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{two_lines})) {
    REQUIRE(std::strncmp(line.str, two_lines_lines[idx], line.len) == 0);
    ++i;
  }
  REQUIRE(i == 2);
}

TEST_CASE("empty_line_cr") {
  int i = 0;
  for_each_line(empty_line, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_lines[i++], line.len) == 0);
  });
  REQUIRE(i == 1);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line})) {
    REQUIRE(std::strncmp(line.str, empty_line_lines[idx], line.len) == 0);
    ++i;
  }
  REQUIRE(i == 1);
}

TEST_CASE("empty_string_cr") {
  int i = 0;
  for_each_line(empty_string, [&](cstr) { i++; });
  REQUIRE(i == 0);

  for (auto const entry : enumerate(lines{empty_string})) {
    (void)entry;
    REQUIRE(false);
  }
}

TEST_CASE("empty_line_between_cr") {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_between_lines[i++], line.len) ==
            0);
  });
  REQUIRE(i == 3);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line_between})) {
    REQUIRE(std::strncmp(line.str, empty_line_between_lines[idx], line.len) ==
            0);
    ++i;
  }
  REQUIRE(i == 3);
}

TEST_CASE("empty_line_between_no_end_cr") {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    REQUIRE(std::strncmp(line.str, empty_line_between_no_end_lines[i++],
                         line.len) == 0);
  });
  REQUIRE(i == 3);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line_between_no_end})) {
    REQUIRE(std::strncmp(line.str, empty_line_between_no_end_lines[idx],
                         line.len) == 0);
    ++i;
  }
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

TEST_CASE("substr_pos_begin_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(0, 2) == "ab");
  REQUIRE(s.substr(0, 2).len == 2);
  REQUIRE(s.substr(0, 2).str == s.str);
}

TEST_CASE("substr_pos_middle_cr") {
  cstr s = "abc";
  REQUIRE(s.substr(1, 2) == "b");
  REQUIRE(s.substr(1, 2).len == 1);
  REQUIRE(s.substr(1, 2).str == s.str + 1);
}

}  // namespace cstr_cr_test
}  // namespace utl
