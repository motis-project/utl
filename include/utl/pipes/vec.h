#pragma once

#include <vector>

#include "utl/clear_t.h"
#include "utl/pipes/make_range.h"

namespace utl {

struct to_vec_t {
  template <typename T>
  friend auto operator|(T&& r, to_vec_t&&) {
    auto it = r.begin();
    std::vector<clear_t<decltype(r.read(it))>> v;
    while (r.valid(it)) {
      v.emplace_back(r.read(it));
      r.next(it);
    }
    return v;
  }
};

inline to_vec_t vec() { return to_vec_t(); }

template <typename Container>
struct to_t {
  template <typename T>
  friend auto operator|(T&& r, to_t&&) {
    auto it = r.begin();
    Container c;
    while (r.valid(it)) {
      c.emplace(r.read(it));
      r.next(it);
    }
    return c;
  }
};

template <typename Container>
inline to_t<Container> to() {
  return {};
}

}  // namespace utl