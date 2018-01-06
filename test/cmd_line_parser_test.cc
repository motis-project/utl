#include "catch.hpp"

#include "utl/cmd_line_parser.h"

using namespace utl;

struct config {
  cmd_line_flag<bool, UTL_SHORT("-c"), UTL_LONG("--capture")> capture;
  cmd_line_flag<std::string, UTL_LONG("--file")> file;
  cmd_line_flag<int, UTL_LONG("--num_threads")> num_threads;
};

TEST_CASE("cmd_line_flag_test") {
  char const* args[] = {"./exe", "-c", "--file", "test", "--num_threads", "8"};
  auto c = parse<config>(sizeof(args) / sizeof(char const*), args);

  CHECK(c.capture.val());
  CHECK(c.file.val() == "test");
  CHECK(c.num_threads.val() == 8);
}
