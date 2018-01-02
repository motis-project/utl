#pragma once

#include <utility>

namespace utl {

struct sum {
  sum() {}

  template <typename Range>
  friend int operator|(Range&& r, sum&& f) {
    auto it = r.begin();
    while (r.valid(it)) {
      f.acc_ += r.read(it);
      r.next(it);
    }
    return f.acc_;
  }

  int acc_ = 0;
};

}  // namespace utl