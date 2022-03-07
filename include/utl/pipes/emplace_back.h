#pragma once

#include "utl/pipes/make_range.h"

namespace utl {

template <typename Dest>
struct empace_back_t {
  template <typename T>
  friend auto operator|(T&& r, empace_back_t&&) {
    auto it = r.begin();
    Dest d;
    while (r.valid(it)) {
      d.emplace_back(r.read(it));
      r.next(it);
    }
    return d;
  }
};

template <typename Container>
inline empace_back_t<Container> emplace_back() {
  return {};
}

} // namespace utl
