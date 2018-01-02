#pragma once

#include <utility>

namespace utl {

struct avg {
  avg() {}

  template <typename Range>
  friend int operator|(Range&& r, avg&& f) {
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