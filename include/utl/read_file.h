#pragma once

#include <fstream>
#include <optional>

namespace utl {

inline std::optional<std::string> read_file(char const* path) {
  try {
    std::ifstream f{path, std::ios::binary | std::ios::ate};
    f.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    auto const size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::string buffer;
    buffer.resize(static_cast<std::size_t>(size));

    return f.read(&buffer[0], size) ? std::make_optional(buffer) : std::nullopt;
  } catch (std::exception const&) {
    return std::nullopt;
  }
}

}  // namespace utl
