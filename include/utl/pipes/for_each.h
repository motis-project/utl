#pragma once

#include <utility>

namespace utl {

template <typename ForEachFn>
struct for_each_t {
  for_each_t(ForEachFn&& f) : fn_(std::forward<ForEachFn>(f)) {}

  template <typename Range>
  friend void operator|(Range&& r, for_each_t&& f) {
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