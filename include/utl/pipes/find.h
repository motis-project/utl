#pragma once

#include <optional>
#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename FindFn>
struct find_t {
  find_t(FindFn&& f) : fn_(std::forward<FindFn>(f)) {}

  template <typename T>
  friend auto operator|(T&& t, find_t&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();
    using value_t =
        std::remove_reference_t<std::remove_const_t<decltype(r.read(it))>>;
    while (r.valid(it)) {
      if (f.fn_(r.read(it))) {
        return std::make_optional<value_t>(r.read(it));
      }
      r.next(it);
    }
    return std::optional<value_t>();
  }

  FindFn fn_;
};

template <typename FindFn>
find_t<FindFn> find(FindFn&& f) {
  return find_t<FindFn>(std::forward<FindFn>(f));
}

}  // namespace utl