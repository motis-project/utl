#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

struct avg {
  template <typename T>
  friend double operator|(T&& t, avg&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();
    while (r.valid(it)) {
      f.sum_ += r.read(it);
      ++f.count_;
      r.next(it);
    }
    return f.sum_ / static_cast<double>(f.count_);
  }

  int sum_ = 0;
  int count_ = 0;
};

}  // namespace utl