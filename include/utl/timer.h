#pragma once

#include <chrono>
#include <string>

namespace utl {

struct scoped_timer final {
  explicit scoped_timer(std::string name);
  scoped_timer(scoped_timer const&) = delete;
  scoped_timer(scoped_timer&&) = delete;
  scoped_timer& operator=(scoped_timer const&) = delete;
  scoped_timer& operator=(scoped_timer&&) = delete;
  ~scoped_timer();

  void print(std::string_view const message) const;

  std::string name_;
  std::chrono::time_point<std::chrono::steady_clock> start_;
};

struct manual_timer final {
  explicit manual_timer(std::string name);
  void stop_and_print() const;
  void print(std::string_view const message) const;

  std::string name_;
  std::chrono::time_point<std::chrono::steady_clock> start_;
};

}  // namespace utl
