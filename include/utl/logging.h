#pragma once

#ifdef LOGGING_HEADER
#include LOGGING_HEADER
#else

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <mutex>
#include <string>

#ifdef _MSC_VER
#define gmt(a, b) gmtime_s(b, a)
#else
#define gmt(a, b) gmtime_r(a, b)
#endif

#define FILE_NAME \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define uLOG(lvl)                                          \
  utl::log() << "[" << utl::str[lvl] << "]"                \
             << "[" << utl::time() << "]"                  \
             << "[" << FILE_NAME << ":" << __LINE__ << "]" \
             << " "

namespace utl {

struct log {
  log() = default;

  log(log const&) = delete;
  log& operator=(log const&) = delete;

  log(log&&) = default;
  log& operator=(log&&) = default;

  template <typename T>
  friend log&& operator<<(log&& l, T&& t) {
    std::cout << std::forward<T&&>(t);
    return std::move(l);
  }

  ~log() { std::cout << std::endl; }
};

enum log_level { emrg, alrt, crit, err, warn, notice, info, debug };

static const char* const str[]{"emrg", "alrt", "crit", "erro",
                               "warn", "note", "info", "debg"};

inline std::string time(time_t const t) {
  char buf[sizeof "2011-10-08t07:07:09z-0430"];
  struct tm result {};
  gmt(&t, &result);
  strftime(buf, sizeof buf, "%FT%TZ%z", &result);
  return buf;
}

inline std::string time() {
  time_t now;
  std::time(&now);
  return time(now);
}

}  // namespace utl

#endif
