#pragma once

#include <fstream>
#include <istream>
#include <string>

namespace utl {

template <typename Fn>
void for_each_line_in_file(std::string const& filename, Fn&& f) {
  std::istream* in;
  std::ifstream f_in;
  if (filename == "-") {
    in = &std::cin;
  } else {
    f_in.open(filename);
    in = &f_in;
  }

  std::string line;
  while (in->peek() != EOF && !in->eof()) {
    std::getline(*in, line);
    f(line);
  }
}

}  // namespace utl
