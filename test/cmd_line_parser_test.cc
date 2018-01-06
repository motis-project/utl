#include "catch.hpp"

#include "utl/cmd_line_parser.h"

using namespace utl;

struct config {
  cmd_line_flag<bool, UTL_SHORT("-c"), UTL_LONG("--capture"),
                UTL_DESC("capture incoming")>
      capture;
  cmd_line_flag<std::string, UTL_LONG("--file"), UTL_DESC("target file")> file;
  cmd_line_flag<int, UTL_LONG("--num_threads"), UTL_DESC("thread pool size")>
      num_threads;
};

constexpr auto const expected_description =
    R"(         --capture -c    capture incoming
            --file       target file
     --num_threads       thread pool size)";

TEST_CASE("cmd_line_flag_test") {
  char const* args[] = {"./exe", "-c", "--file", "test", "--num_threads", "8"};
  auto c = parse<config>(sizeof(args) / sizeof(char const*), args);

  CHECK(c.capture.val());
  CHECK(c.file.val() == "test");
  CHECK(c.num_threads.val() == 8);
  CHECK(description<config>() == expected_description);
}
