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
template <llvl LogLevel, typename... Args>
struct log {
  log(fmt::format_string<Args...> fmt_str, const Args&&... args,
      std::source_location const& loc = std::source_location::current())
      : loc_{loc},
        ctx_{nullptr},
        msg_{fmt::format(fmt_str, std::forward<Args>(args)...)} {}

  ~log() {
    if (LogLevel >= utl::s_verbosity) {
#if defined(_WIN32)
      const char* baseFileName = strrchr(loc_.file_name(), '\\')
                                     ? strrchr(loc_.file_name(), '\\') + 1
                                     : loc_.file_name();
#else
      const char* baseFileName = strrchr(loc_.file_name(), '/')
                                     ? strrchr(loc_.file_name(), '/') + 1
                                     : loc_.file_name();
#endif
      fmt::print(std::clog, "{time} [{level}] [{file}:{line}]{ctx} {msg}\n",
                 fmt::arg("time", now()), fmt::arg("level", to_str(LogLevel)),
                 fmt::arg("file", baseFileName), fmt::arg("line", loc_.line()),
                 //  fmt::arg("fn", loc_.function_name()),
                 fmt::arg("ctx", ctx_ ? (std::string(" [") + std::string(ctx_) +
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
  std::source_location loc_;
  char const* ctx_;
  std::string msg_;
  std::initializer_list<std::pair<std::string_view, std::string_view> >
      metadata_;
};

template <typename... Args>
struct debug : public log<llvl::debug, Args...> {
  using log<llvl::debug, Args...>::log;
};

template <typename... Args>
struct info : public log<llvl::info, Args...> {
  using log<llvl::info, Args...>::log;
};

template <typename... Args>
struct error : public log<llvl::error, Args...> {
  using log<llvl::error, Args...>::log;
};

// Template deduction guides, to help the compiler distinguish between
// the variadic template Args... and the next argument std::source_location
// which has a default value:

template <typename... Args>
debug(fmt::format_string<Args...>, Args&&... args) -> debug<Args...>;

template <typename... Args>
info(fmt::format_string<Args...>, Args&&... args) -> info<Args...>;

template <typename... Args>
error(fmt::format_string<Args...>, Args&&... args) -> error<Args...>;

}  // namespace utl

#endif  // LOGGING_HEADER
