#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

struct sum {
  sum() {}

  template <typename T>
  friend auto operator|(T&& t, sum&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();
    while (r.valid(it)) {
      f.acc_ += r.read(it);
      r.next(it);
    }
    return f.acc_;
  }

  std::size_t acc_ = 0;
};

}  // namespace utl