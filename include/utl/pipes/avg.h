#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

struct avg_t {
  template <typename T>
  friend double operator|(T&& r, avg_t) {
    auto sum = std::size_t{0};
    auto count = std::size_t{0};
    auto it = r.begin();
    while (r.valid(it)) {
      sum += r.read(it);
      ++count;
      r.next(it);
    }
    return sum / static_cast<double>(count);
  }
};

inline avg_t avg() { return {}; }

}  // namespace utl