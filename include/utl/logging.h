#pragma once

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
enum class log_level { debug, info, error };

constexpr char const* to_str(log_level const level) {
  switch (level) {
    case log_level::debug: return "debug";
    case log_level::info: return "info";
    case log_level::error: return "error";
  }
  return "";
}

extern log_level log_verbosity;

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

/// Produce a new log line at the given `level`.
template <log_level LogLevel, typename... Args>
struct log {
  log(const char* ctx, fmt::format_string<Args...> fmt_str, Args&&... args,
      std::source_location const& loc = std::source_location::current())
      : loc_{loc},
        ctx_{ctx},
        msg_{fmt::format(fmt_str, std::forward<Args>(args)...)} {}

  ~log() {
    if (LogLevel >= log_verbosity) {
#if defined(_WIN32)
      auto const base_file_name = strrchr(loc_.file_name(), '\\')
                                      ? strrchr(loc_.file_name(), '\\') + 1
                                      : loc_.file_name();
#else
      // On MacOS, due to a bug with Clang 15, the wrong filename
      // is retrieved (logging.h instead of the calling file):
      // https://github.com/llvm/llvm-project/issues/56379
      auto const base_file_name = strrchr(loc_.file_name(), '/')
                                      ? strrchr(loc_.file_name(), '/') + 1
                                      : loc_.file_name();
#endif
      fmt::print(std::clog, "{time} [{level}] [{file}:{line}] [{ctx}] {msg}\n",
                 fmt::arg("time", now()), fmt::arg("level", to_str(LogLevel)),
                 fmt::arg("file", base_file_name),
                 fmt::arg("line", loc_.line()), fmt::arg("ctx", ctx_),
                 fmt::arg("msg", msg_));
    }
  }

  /// Add key-values metadata
  void attrs(
      std::initializer_list<std::pair<std::string_view, std::string_view> >&&
          attrs) {
    attrs_ = std::move(attrs);
  }

  log_level level_;
  std::source_location loc_;
  char const* ctx_;
  std::string msg_;
  std::initializer_list<std::pair<std::string_view, std::string_view> > attrs_;
};

/// Produce a new DEBUG log line
template <typename... Args>
struct log_debug : public log<log_level::debug, Args...> {
  using log<log_level::debug, Args...>::log;
};

/// Produce a new INFO log line
template <typename... Args>
struct log_info : public log<log_level::info, Args...> {
  using log<log_level::info, Args...>::log;
};

/// Produce a new ERROR log line
template <typename... Args>
struct log_error : public log<log_level::error, Args...> {
  using log<log_level::error, Args...>::log;
};

// Template deduction guides, to help the compiler distinguish between
// the variadic template Args... and the next argument std::source_location
// which has a default value:

template <typename... Args>
log_debug(const char* ctx, fmt::format_string<Args...>,
          Args&&... args) -> log_debug<Args...>;

template <typename... Args>
log_info(const char* ctx, fmt::format_string<Args...>,
         Args&&... args) -> log_info<Args...>;

template <typename... Args>
log_error(const char* ctx, fmt::format_string<Args...>,
          Args&&... args) -> log_error<Args...>;

}  // namespace utl
