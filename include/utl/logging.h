#pragma once

#ifdef LOGGING_HEADER
#include LOGGING_HEADER
#else

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "fmt/core.h"
#include "fmt/ostream.h"

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

#define FILE_AND_LINE (__FILE__ ":" STRINGIFY(__LINE__))
#if defined(_WIN32)
#define FILE_AND_LINE_SHORT                                        \
  (strrchr(FILE_AND_LINE, '\\') ? strrchr(FILE_AND_LINE, '\\') + 1 \
                                : FILE_AND_LINE)
#else
#define FILE_AND_LINE_SHORT                                      \
  (strrchr(FILE_AND_LINE, '/') ? strrchr(FILE_AND_LINE, '/') + 1 \
                               : FILE_AND_LINE)
#endif

namespace utl {

enum class log_level { debug, info, error };

constexpr char const* to_str(log_level const level) {
  switch (level) {
    case log_level::debug: return "debug";
    case log_level::info: return "info";
    case log_level::error: return "error";
  }
  return "";
}

static log_level s_verbosity;

inline std::string now() {
  using clock = std::chrono::system_clock;
  auto const now = clock::to_time_t(clock::now());
  struct tm tmp {};
#if _MSC_VER >= 1400
  gmtime_s(&tmp, &now);
#else
  gmtime_r(&now, &tmp);
#endif

  std::stringstream ss;
  ss << std::put_time(&tmp, "%FT%TZ");
  return ss.str();
}

/// Produce a new log line at the given `level`, with the given prefix `ctx` and
/// message
template <typename... Args>
void log(log_level const level, char const* ctx,
         fmt::format_string<Args...> fmt_str, Args&&... args) {
  if (level >= ::utl::s_verbosity) {
    fmt::print(std::clog, "{time} [{level}] [{ctx}] {msg}\n",
               fmt::arg("time", now()), fmt::arg("level", to_str(level)),
               fmt::arg("ctx", ctx),
               fmt::arg("msg", fmt::format(fmt::runtime(fmt_str),
                                           std::forward<Args>(args)...)));
  }
}

}  // namespace utl

// clang-format off
#define _CLANG_C_COMPILER clang-cl
// clang-format on

/**
 * Shorthand to invoke utl::log without specifying the namespace
 */
#if _MSC_VER && CMAKE_C_COMPILER != _CLANG_C_COMPILER
#define log(level, ctx, fmt_str, ...) \
  utl::log(utl::log_level::##level, ctx, fmt_str, __VA_ARGS__)
#else
#define log(level, ctx, fmt_str, ...) \
  utl::log(utl::log_level::level, ctx, fmt_str, ##__VA_ARGS__)
#endif

/**
 * Invoke utl::log using the current C++ filename & line number as ctx
 */
#if _MSC_VER && CMAKE_C_COMPILER != _CLANG_C_COMPILER
#define logF(level, fmt_str, ...) \
  log(level, FILE_AND_LINE_SHORT, fmt_str, __VA_ARGS__)
#else
#define logF(level, fmt_str, ...) \
  log(level, FILE_AND_LINE_SHORT, fmt_str, ##__VA_ARGS__)
#endif

#endif  // LOGGING_HEADER
