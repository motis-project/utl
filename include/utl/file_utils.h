#pragma once

#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>

#include "fmt/std.h"

#include "verify.h"

namespace utl {

inline std::ifstream open_file(std::filesystem::path const& p) {
  auto f = std::ifstream{};
  f.exceptions(std::ios_base::failbit | std::ios_base::badbit);
  try {
    f.open(p);
  } catch (std::exception const& e) {
    throw utl::fail("could not open file \"{}\": {}", p, e.what());
  }
  return f;
};

inline std::optional<std::string> read_line(std::ifstream& f) {
  if (f.eof() || f.peek() == EOF) {
    return std::nullopt;
  }
  std::string line;
  std::getline(f, line);
  return line;
};

}  // namespace utl