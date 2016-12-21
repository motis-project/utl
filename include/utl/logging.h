#pragma once

#ifdef LOGGING_HEADER
#include LOGGING_HEADER
#else

#include <ctime>
#include <iomanip>
#include <iostream>

namespace utl {

struct log {
  log() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::gmtime(&t);
    std::clog << "[" << std::put_time(&tm, "%FZ%TZ") << "] ";
  }
  ~log() { std::clog << std::endl; }

  template <typename T>
  log& operator<<(T const& el) {
    std::clog << el;
    return *this;
  }
};

}  // namespace utl

#define uLOG() utl::log()

#endif
