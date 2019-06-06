#pragma once

#include <stdexcept>

#include "fmt/format.h"

namespace utl {

template <typename Msg, typename... FmtArgs>
std::runtime_error fail(Msg&& msg, FmtArgs... args) {
  return std::runtime_error{fmt::format(msg, std::forward<FmtArgs>(args)...)};
}

template <typename Msg, typename... FmtArgs>
void verify(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    fmt::print(msg, std::forward<FmtArgs>(args)...);
    throw fail(msg, std::forward<FmtArgs>(args)...);
  }
}

template <typename Msg, typename... FmtArgs>
void verify_silent(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    throw fail(msg, std::forward<FmtArgs>(args)...);
  }
}

}  // namespace utl
