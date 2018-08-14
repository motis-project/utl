#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename ForEachFn>
struct for_each_t {
  for_each_t(ForEachFn&& f) : fn_(std::forward<ForEachFn>(f)) {}

  template <typename T>
  friend void operator|(T&& t, for_each_t&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();
    while (r.valid(it)) {
      f.fn_(r.read(it));
      r.next(it);
    }
  }

  ForEachFn fn_;
};

template <typename ForEachFn>
for_each_t<ForEachFn> for_each(ForEachFn&& f) {
  return for_each_t<ForEachFn>(std::forward<ForEachFn>(f));
}

}  // namespace utl