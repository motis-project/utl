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

#if defined(_WIN32)
auto const FILE_SEPARATOR = '\\';
auto const MODULE_PREFIX = "\\motis-";
#else
auto const FILE_SEPARATOR = '/';
auto const MODULE_PREFIX = "/motis-";
#endif

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

/// Produce a new log line at the given `level`.
template <log_level LogLevel, typename... Args>
struct log {
  log(const char* ctx, fmt::format_string<Args...> fmt_str,
      const Args&&... args,
      std::source_location const& loc = std::source_location::current())
      : loc_{loc},
        ctx_{ctx},
        msg_{fmt::format(fmt_str, std::forward<Args>(args)...)} {}

  ~log() {
    if (LogLevel >= utl::s_verbosity) {
      // On MacOS, due to a bug with Clang 15, the wrong filename
      // is retrieved (logging.h instead of the calling file):
      // https://github.com/llvm/llvm-project/issues/56379
      auto const char_index = strrchr(loc_.file_name(), FILE_SEPARATOR);
      auto const base_file_name =
          char_index ? char_index + 1 : loc_.file_name();
      auto const file_name = std::string(loc_.file_name());
      auto const mod_start = file_name.find(MODULE_PREFIX) + 7;
      auto const mod_end = file_name.find(FILE_SEPARATOR, mod_start);
      auto const mod_name = file_name.substr(mod_start, mod_end - mod_start);
      fmt::print(
          std::clog,
          "{time} [{level}] [{mod}] [{file}:{line} {fn}] [{ctx}] {msg}\n",
          fmt::arg("time", now()), fmt::arg("level", to_str(LogLevel)),
          fmt::arg("mod", mod_name), fmt::arg("file", base_file_name),
          fmt::arg("line", loc_.line()), fmt::arg("fn", loc_.function_name()),
          fmt::arg("ctx", ctx_), fmt::arg("msg", msg_));
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
struct debug : public log<log_level::debug, Args...> {
  using log<log_level::debug, Args...>::log;
};

/// Produce a new INFO log line
template <typename... Args>
struct info : public log<log_level::info, Args...> {
  using log<log_level::info, Args...>::log;
};

/// Produce a new ERROR log line
template <typename... Args>
struct error : public log<log_level::error, Args...> {
  using log<log_level::error, Args...>::log;
};

// Template deduction guides, to help the compiler distinguish between
// the variadic template Args... and the next argument std::source_location
// which has a default value:

template <typename... Args>
debug(const char* ctx, fmt::format_string<Args...>,
      Args&&... args) -> debug<Args...>;

template <typename... Args>
info(const char* ctx, fmt::format_string<Args...>,
     Args&&... args) -> info<Args...>;

template <typename... Args>
error(const char* ctx, fmt::format_string<Args...>,
      Args&&... args) -> error<Args...>;

}  // namespace utl

#endif  // LOGGING_HEADER
