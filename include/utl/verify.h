#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "fmt/ostream.h"

#define utl_verify(condition, fmt_str, ...)                 \
  {                                                         \
    if (!(condition)) {                                     \
      [[unlikely]] throw ::utl::fail(fmt_str, __VA_ARGS__); \
    }                                                       \
  }

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

  fmt::print(std::clog, "{} [VERIFY FAIL] ", std::put_time(&tmp, "%FT%TZ"));
  fmt::print(std::clog, std::forward<Msg>(msg), std::forward<FmtArgs>(args)...);
  fmt::print(std::clog, "\n");

  return std::runtime_error{
      fmt::format(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...)};
}

template <typename Msg, typename... FmtArgs>
void verify(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    [[unlikely]] throw fail(std::forward<Msg>(msg),
                            std::forward<FmtArgs>(args)...);
  }
}

template <typename Msg, typename... FmtArgs>
void verify_silent(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    [[unlikely]] throw std::runtime_error{
        fmt::format(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...)};
  }
}

template <typename Exception>
void verify_ex(bool condition, Exception&& ex) {
  if (!condition) {
    [[unlikely]] throw ex;
  }
}

}  // namespace utl
