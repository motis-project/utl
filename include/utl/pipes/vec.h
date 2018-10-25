#pragma once

#include <vector>

#include "utl/pipes/make_range.h"

namespace utl {

struct to_vec_t {
  template <typename T>
  friend auto operator|(T&& t, to_vec_t&&) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();
    using value_t =
        std::remove_const_t<std::remove_reference_t<decltype(r.read(it))>>;
    std::vector<value_t> v;
    while (r.valid(it)) {
      v.emplace_back(r.read(it));
      r.next(it);
    }
    return v;
  }
};

inline to_vec_t vec() { return to_vec_t(); }

}  // namespace utl