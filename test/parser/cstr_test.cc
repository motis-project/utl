#include "gtest/gtest.h"

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

TEST(cstr_test, substr_overflow) {
  cstr s = "012345678";

  ASSERT_TRUE(s.substr(10, 12).len == 0U);
  ASSERT_TRUE(s.substr(5, 10) == cstr{"5678"});
  ASSERT_TRUE(s.substr(5, 10).len == 4);

  ASSERT_TRUE(s.substr(10, size{2}).len == 0U);
  ASSERT_TRUE(s.substr(5, size{5}) == cstr{"5678"});
  ASSERT_TRUE(s.substr(5, size{5}).len == 4);
}

TEST(cstr_test, get_line) {
  cstr str = one_line;
  auto line = get_line(str);
  ASSERT_TRUE(line.len == 1);
  ASSERT_TRUE(std::strncmp(line.str, "1", 1) == 0);
  ASSERT_TRUE(std::strlen(line.str) == 2);
}

TEST(cstr_test, one_line) {
  int i = 0;
  for_each_line(one_line, [&](cstr line) {
    ASSERT_TRUE(i < 2);
    ASSERT_TRUE(
        std::strncmp(line.str, one_line_lines[i++], line.len) ==  // NOLINT
        0);
  });
  ASSERT_TRUE(i == 1);
}

TEST(cstr_test, one_line_no_end) {
  int i = 0;
  for_each_line(one_line_no_end, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, one_line_no_end_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 1);
}

TEST(cstr_test, two_lines) {
  int i = 0;
  for_each_line(two_lines, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, two_lines_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 2);
}

TEST(cstr_test, empty_line) {
  int i = 0;
  for_each_line(empty_line, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, empty_line_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 1);
}

TEST(cstr_test, empty_string) {
  int i = 0;
  for_each_line(empty_string, [&](cstr) { ++i; });
  ASSERT_TRUE(i == 0);
}

TEST(cstr_test, empty_line_between) {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    ASSERT_TRUE(std::strncmp(line.str, empty_line_between_lines[i++],  // NOLINT
                             line.len) == 0);
  });
  ASSERT_TRUE(i == 3);
}

TEST(cstr_test, empty_line_between_no_end) {
  int i = 0;
  for_each_line(empty_line_between_no_end, [&](cstr line) {
    ASSERT_TRUE(std::strncmp(line.str,  // NOLINT
                             empty_line_between_no_end_lines[i++],
                             line.len) == 0);
  });
  ASSERT_TRUE(i == 3);
}

TEST(cstr_test, substr_len_begin) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(0, size(2)) == "ab");
  ASSERT_TRUE(s.substr(0, size(2)).len == 2);
  ASSERT_TRUE(s.substr(0, size(2)).str == s.str);
}

TEST(cstr_test, substr_len_middle) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(1, size(1)) == "b");
  ASSERT_TRUE(s.substr(1, size(1)).len == 1);
  ASSERT_TRUE(s.substr(1, size(1)).str == s.str + 1);
}

TEST(cstr_test, sbustr_pos_begin) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(0, 2) == "ab");
  ASSERT_TRUE(s.substr(0, 2).len == 2);
  ASSERT_TRUE(s.substr(0, 2).str == s.str);
}

TEST(cstr_test, sbustr_pos_middle) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(1, 2) == "b");
  ASSERT_TRUE(s.substr(1, 2).len == 1);
  ASSERT_TRUE(s.substr(1, 2).str == s.str + 1);
}

}  // namespace utl::cstr_test
