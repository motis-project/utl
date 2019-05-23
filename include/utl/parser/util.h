#pragma once

#include <chrono>
#include <cstdio>
#include <ctime>
#include <stdexcept>

#include "fmt/format.h"

namespace utl {

template <typename Msg, typename... FmtArgs>
inline void log_err(Msg&& msg, FmtArgs... args) {
  fmt::print(msg, std::forward<FmtArgs>(args)...);
}

template <typename Msg, typename... FmtArgs>
inline void log_info(Msg&& msg, FmtArgs... args) {
  fmt::print(msg, std::forward<FmtArgs>(args)...);
}

template <typename NameType, typename Fn>
inline void log_time(NameType&& name, Fn&& fn) {
  auto start = std::chrono::steady_clock::now();
  fn();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start)
                .count();
  log_info("%s took %lldms", name, ms);
}

template <typename Msg, typename... FmtArgs>
inline void verify(bool condition, Msg&& msg, FmtArgs... args) {
  if (!condition) {
    log_info(std::forward<Msg>(msg), std::forward<FmtArgs>(args)...);
    throw std::runtime_error(msg);
  }
}

}  // namespace utl