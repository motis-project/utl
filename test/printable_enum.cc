#include "gtest/gtest.h"

#include <sstream>

#include "utl/printable_enum.h"

PRINTABLE_ENUM(color, RED, GREEN, BLUE);

TEST(printable_enum, print) {
  EXPECT_TRUE(as_string(color::RED) == "RED");
  EXPECT_TRUE(as_string(color::GREEN) == "GREEN");
  EXPECT_TRUE(as_string(color::BLUE) == "BLUE");

  std::stringstream ss;
  ss << color::RED << color::GREEN << color::BLUE;
  EXPECT_TRUE(ss.str() == "REDGREENBLUE");
}
