#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename CountFn>
struct count_t {
  count_t(CountFn&& f) : fn_(std::forward<CountFn>(f)) {}

  template <typename T>
  friend auto operator|(T&& t, count_t&& f) {
    auto r = make_range(std::forward<T>(t));
    auto it = r.begin();

    while (r.valid(it)) {
      f.count_ += (f.fn_(r.read(it)) ? 1 : 0);
      r.next(it);
    }

    return f.count_;
  }

  std::size_t count_{0};
  CountFn fn_;
};

template <typename CountFn>
count_t<CountFn> count(CountFn&& f) {
  return count_t<CountFn>(std::forward<CountFn>(f));
}

}  // namespace utl
