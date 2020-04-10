#pragma once

#include <map>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename Dest>
struct emplace_t {
  template <typename T>
  friend auto operator|(T&& t, emplace_t&&) {
    auto&& r = make_range(std::forward<T>(t));
    auto it = r.begin();
    Dest v;
    while (r.valid(it)) {
      auto entry = r.read(it);
      v.emplace(entry);
      r.next(it);
    }
    return v;
  }
};

template <typename Dest>
inline emplace_t<Dest> emplace() {
  return emplace_t<Dest>();
}

}  // namespace utl