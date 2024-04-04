#include "gtest/gtest.h"

#include "utl/enumerate.h"
#include "utl/parser/cstr.h"

namespace utl::cstr_cr_test {

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

TEST(cstr_cr_test, line_iterator_test) {
  line_iterator it{empty_line_between_no_end};
  line_iterator end;

  ASSERT_TRUE(it != end);
  ASSERT_TRUE(*it == empty_line_between_no_end_lines[0]);
  ++it;

  ASSERT_TRUE(it != end);
  ASSERT_TRUE(*it == empty_line_between_no_end_lines[1]);
  ++it;

  ASSERT_TRUE(it != end);
  ASSERT_TRUE(*it == empty_line_between_no_end_lines[2]);
  ++it;

  ASSERT_TRUE(it == end);
}

TEST(cstr_cr_test, get_line_cr) {
  cstr str = one_line;
  auto line = get_line(str);
  ASSERT_TRUE(line.len == 1);
  ASSERT_TRUE(std::strncmp(line.str, "1", 1) == 0);
  ASSERT_TRUE(std::strlen(line.str) == 3);
}

TEST(cstr_cr_test, one_line_cr) {
  int i = 0;
  for_each_line(one_line, [&](cstr line) {
    ASSERT_TRUE(std::strncmp(line.str, one_line_lines[i++], line.len)  // NOLINT
                == 0);
  });
  ASSERT_TRUE(i == 1);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{one_line})) {
    ASSERT_TRUE(std::strncmp(line.str, one_line_lines[idx], line.len)  // NOLINT
                == 0);
    ++i;
  }
  ASSERT_TRUE(i == 1);
}

TEST(cstr_cr_test, one_line_no_end_cr) {
  int i = 0;
  for_each_line(one_line_no_end, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, one_line_no_end_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 1);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{one_line_no_end})) {
    ASSERT_TRUE(
        std::strncmp(line.str, one_line_no_end_lines[idx], line.len)  // NOLINT
        == 0);
    ++i;
  }
  ASSERT_TRUE(i == 1);
}

TEST(cstr_cr_test, two_lines_cr) {
  int i = 0;
  for_each_line(two_lines, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, two_lines_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 2);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{two_lines})) {
    ASSERT_TRUE(
        std::strncmp(line.str, two_lines_lines[idx], line.len)  // NOLINT
        == 0);
    ++i;
  }
  ASSERT_TRUE(i == 2);
}

TEST(cstr_cr_test, empty_line_cr) {
  int i = 0;
  for_each_line(empty_line, [&](cstr line) {
    ASSERT_TRUE(
        std::strncmp(line.str, empty_line_lines[i++], line.len)  // NOLINT
        == 0);
  });
  ASSERT_TRUE(i == 1);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line})) {
    ASSERT_TRUE(
        std::strncmp(line.str, empty_line_lines[idx], line.len)  // NOLINT
        == 0);
    ++i;
  }
  ASSERT_TRUE(i == 1);
}

TEST(cstr_cr_test, empty_string_cr) {
  int i = 0;
  for_each_line(empty_string, [&](cstr) { i++; });
  ASSERT_TRUE(i == 0);

  for (auto const entry : enumerate(lines{empty_string})) {
    (void)entry;
    ASSERT_TRUE(false);
  }
}

TEST(cstr_cr_test, empty_line_between_cr) {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    ASSERT_TRUE(std::strncmp(line.str, empty_line_between_lines[i++],  // NOLINT
                             line.len) == 0);
  });
  ASSERT_TRUE(i == 3);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line_between})) {
    ASSERT_TRUE(std::strncmp(line.str, empty_line_between_lines[idx],  // NOLINT
                             line.len) == 0);
    ++i;
  }
  ASSERT_TRUE(i == 3);
}

TEST(cstr_cr_test, empty_line_between_no_end_cr) {
  int i = 0;
  for_each_line(empty_line_between, [&](cstr line) {
    ASSERT_TRUE(std::strncmp(line.str,  // NOLINT
                             empty_line_between_no_end_lines[i++],
                             line.len) == 0);
  });
  ASSERT_TRUE(i == 3);

  i = 0;
  for (auto const [idx, line] : enumerate(lines{empty_line_between_no_end})) {
    ASSERT_TRUE(std::strncmp(line.str,  // NOLINT
                             empty_line_between_no_end_lines[idx],
                             line.len) == 0);
    ++i;
  }
  ASSERT_TRUE(i == 3);
}

TEST(cstr_cr_test, substr_len_begin_cr) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(0, size(2)) == "ab");
  ASSERT_TRUE(s.substr(0, size(2)).len == 2);
  ASSERT_TRUE(s.substr(0, size(2)).str == s.str);
}

TEST(cstr_cr_test, substr_len_middle_cr) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(1, size(1)) == "b");
  ASSERT_TRUE(s.substr(1, size(1)).len == 1);
  ASSERT_TRUE(s.substr(1, size(1)).str == s.str + 1);
}

TEST(cstr_cr_test, substr_pos_begin_cr) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(0, 2) == "ab");
  ASSERT_TRUE(s.substr(0, 2).len == 2);
  ASSERT_TRUE(s.substr(0, 2).str == s.str);
}

TEST(cstr_cr_test, substr_pos_middle_cr) {
  cstr s = "abc";
  ASSERT_TRUE(s.substr(1, 2) == "b");
  ASSERT_TRUE(s.substr(1, 2).len == 1);
  ASSERT_TRUE(s.substr(1, 2).str == s.str + 1);
}

}  // namespace utl::cstr_cr_test
