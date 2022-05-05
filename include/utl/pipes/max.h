#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename MaxType>
struct max {
  template <typename T>
  friend double operator|(T&& t, max&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();

    while (r.valid(it)) {
      f.max_ = std::max(f.max_, r.read(it));
      r.next(it);
    }

    return f.max_;
  }

  MaxType max_;
};

}  // namespace utl
