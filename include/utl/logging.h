#pragma once

#ifdef LOGGING_HEADER
#include LOGGING_HEADER
#else

#include <chrono>
#include <iomanip>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>

#include "fmt/core.h"
#include "fmt/ostream.h"

namespace utl {

/// Log level
enum class llvl { debug, info, error };

constexpr char const* to_str(llvl const level) {
  switch (level) {
    case llvl::debug: return "debug";
    case llvl::info: return "info";
    case llvl::error: return "error";
  }
  return "";
}

static llvl s_verbosity;

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

/// Produce a new log line at the given `level`, with the given message
template <typename... T>
struct log {
  log(llvl const level, fmt::format_string<T...> fmt_str, const T&&... args)
      //, std::source_location const& loc = std::source_location::current())
      : level_{level},
        // loc_{loc},
        ctx_{nullptr},
        msg_{fmt::format(fmt_str, std::forward<T>(args)...)} {}

  ~log() {
    if (level_ >= utl::s_verbosity) {
      fmt::print(std::clog,
                 "{time} [{level}] [{file}:{line} {fn}] {ctx} {msg}\n",
                 fmt::arg("time", now()), fmt::arg("level", to_str(level_)),
                 // fmt::arg("file", loc_.file_name()),
                 // fmt::arg("line", loc_.line()),
                 // fmt::arg("fn", loc_.function_name()),
                 fmt::arg("ctx", ctx_ ? (std::string("[") + std::string(ctx_) +
                                         std::string("]"))
                                      : ""),
                 fmt::arg("msg", msg_));
    }
  }

  void ctx(char const* ctx) { ctx_ = ctx; }

  void metadata(
      std::initializer_list<std::pair<std::string_view, std::string_view> >&&
          metadata) {
    metadata_ = std::move(metadata);
  }

  llvl level_;
  // std::source_location loc_;
  char const* ctx_;
  std::string msg_;
  std::initializer_list<std::pair<std::string_view, std::string_view> >
      metadata_;
};

// Class template deduction guide, to help the compiler distinguish between the
// variadic template Args... and the next argument std::source_location which
// has a default value:
// template <typename... Args> log(llvl level, char const*,
// fmt::format_string<Args...>, Args&&... args) -> log<Args...>;

template <typename... T>
auto debug(fmt::format_string<T...> fmt_str, const T&&... args) {
  //, std::source_location const& loc = std::source_location::current())
  return log(llvl::debug, fmt_str, std::forward<T>(args)...);
}

template <typename... T>
auto info(fmt::format_string<T...> fmt_str, const T&&... args) {
  //, std::source_location const& loc = std::source_location::current())
  return log(llvl::info, fmt_str, std::forward<T>(args)...);
}

template <typename... T>
auto error(fmt::format_string<T...> fmt_str, const T&&... args) {
  //, std::source_location const& loc = std::source_location::current())
  return log(llvl::error, fmt_str, std::forward<T>(args)...);
}

}  // namespace utl

#endif  // LOGGING_HEADER
