#pragma once

#include <map>

#include "utl/pipes/make_range.h"

namespace utl {

struct to_map_t {
  template <typename T>
  friend auto operator|(T&& t, to_map_t&&) {
    auto&& r = make_range(std::forward<T>(t));
    auto it = r.begin();
    using value_t = decltype(r.read(it));
    std::map<typename value_t::first_type, typename value_t::second_type> v;
    while (r.valid(it)) {
      auto entry = r.read(it);
      v.emplace(std::move(entry.first), std::move(entry.second));
      r.next(it);
    }
    return v;
  }
};

inline to_map_t map() { return to_map_t(); }

}  // namespace utl