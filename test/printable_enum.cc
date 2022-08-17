#include "catch2/catch_all.hpp"

#include <sstream>

#include "utl/printable_enum.h"

PRINTABLE_ENUM(color, RED, GREEN, BLUE);

TEST_CASE("printable_enum") {
  CHECK(as_string(color::RED) == "RED");
  CHECK(as_string(color::GREEN) == "GREEN");
  CHECK(as_string(color::BLUE) == "BLUE");

  std::stringstream ss;
  ss << color::RED << color::GREEN << color::BLUE;
  CHECK(ss.str() == "REDGREENBLUE");
}
