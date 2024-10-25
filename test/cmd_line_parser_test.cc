#include "gtest/gtest.h"

#include "utl/cmd_line_parser.h"

using namespace utl;

struct config {
  cmd_line_flag<bool, UTL_SHORT("-c"), UTL_LONG("--capture"),
                UTL_DESC("capture incoming")>
      capture_;
  cmd_line_flag<std::string, required, UTL_LONG("--file"),
                UTL_DESC("target file")>
      file_;
  cmd_line_flag<int, UTL_LONG("--num_threads"), UTL_DESC("thread pool size")>
      num_threads_;
};

constexpr auto const expected_description =
    R"(       --capture -c      capture incoming
          --file       * target file
   --num_threads         thread pool size)";

TEST(cmd, cmd_line_flag_test) {
  char const* args[] = {"./exe", "-c", "--file", "test", "--num_threads", "8"};
  auto c = parse_flags<config>(sizeof(args) / sizeof(char const*), args);

  EXPECT_TRUE(c.capture_.val());
  EXPECT_TRUE(c.file_.val() == "test");
  EXPECT_TRUE(c.num_threads_.val() == 8);
  EXPECT_TRUE(description<config>() == expected_description);
}

TEST(cmd, cmd_line_flag_required_test) {
  char const* args[] = {"./exe", "-c", "--num_threads", "8"};

  bool thrown = false;
  try {
    parse_flags<config>(sizeof(args) / sizeof(char const*), args);
  } catch (...) {
    thrown = true;
  }
  EXPECT_TRUE(thrown);
}
