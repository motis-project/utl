#pragma once

#include "utl/pipes/make_range.h"

namespace utl {

template <typename Dest>
struct insert_t {
  template <typename T>
  friend auto operator|(T&& t, insert_t&&) {
    auto&& r = make_range(std::forward<T>(t));
    auto it = r.begin();
    Dest v;
    while (r.valid(it)) {
      auto entry = r.read(it);
      v.insert(entry);
      r.next(it);
    }
    return v;
  }
};

template <typename Dest>
inline insert_t<Dest> insert() {
  return insert_t<Dest>();
}

} // namespace utl
