#pragma once

#include <chrono>
#include <iomanip>
#include <stdexcept>

#include "fmt/ostream.h"

namespace utl {

template <typename Msg, typename... FmtArgs>
std::runtime_error fail(Msg&& msg, FmtArgs... args) {
  using clock = std::chrono::system_clock;

  auto const now = clock::to_time_t(clock::now());
  struct tm tmp;
#if _MSC_VER >= 1400
  gmtime_s(&tmp, &now);
#else
  gmtime_r(&now, &tmp);
#endif

  fmt::print("{} [VERIFY FAIL] ", std::put_time(&tmp, "%FT%TZ"));
  fmt::print(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...);
  fmt::print("\n");

  return std::runtime_error{
      fmt::format(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...)};
}

template <typename Msg, typename... FmtArgs>
void verify(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    throw fail(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...);
  }
}

template <typename Msg, typename... FmtArgs>
void verify_silent(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    throw std::runtime_error{
        fmt::format(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...)};
  }
}

}  // namespace utl
