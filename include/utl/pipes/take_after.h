#pragma once

#include <utility>

#include "utl/clear_t.h"
#include "utl/pipes/make_range.h"

namespace utl {

template <typename Range, typename TakeAfter>
struct take_after_range : public clear_t<Range> {
  using parent_t = clear_t<Range>;

  take_after_range(Range&& r, TakeAfter&& take_after)
      : parent_t(std::forward<parent_t>(r)),
        take_after_(std::forward<TakeAfter>(take_after)) {}

  auto begin() {
    auto it = parent_t::begin();
    while (!take_after_.fn_(this->read(it))) {
      parent_t::next(it);
    }
    return it;
  }

  TakeAfter take_after_;
};

template <typename Fn>
struct take_after_t {
  take_after_t(Fn&& f) : fn_(std::forward<Fn>(f)) {}

  template <typename T>
  friend auto operator|(T&& r, take_after_t&& f) {
    return take_after_range<decltype(make_range(r)), take_after_t>(
        make_range(std::forward<T>(r)), std::move(f));
  }

  Fn fn_;
};

template <typename TakeAfter>
take_after_t<TakeAfter> take_after(TakeAfter&& f) {
  return take_after_t<TakeAfter>(std::forward<TakeAfter>(f));
}

template <typename Range, typename TakeAfter>
struct is_range<take_after_range<Range, TakeAfter>> : std::true_type {};

}  // namespace utl
