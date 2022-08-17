#pragma once

#include <utility>

#include "utl/clear_t.h"
#include "utl/pipes/make_range.h"

namespace utl {

template <typename Range>
struct unique_range : public clear_t<Range> {
  using parent_t = clear_t<Range>;

  unique_range(Range&& r) : parent_t(std::forward<parent_t>(r)) {}

  template <typename It>
  void find(It& it) {
    while (this->valid(it) && parent_t::read(it) == pred_) {
      parent_t::next(it);
    }
    if (this->valid(it)) {
      pred_ = parent_t::read(it);
    }
  }

  template <typename It>
  void next(It& it) {
    parent_t::next(it);
    find(it);
  }

  auto begin() {
    auto it = parent_t::begin();
    if (this->valid(it)) {
      pred_ = parent_t::read(it);
    }
    return it;
  }

  mutable typename parent_t::result_t pred_{};
};

struct unique_t {
  template <typename T>
  friend auto operator|(T&& r, unique_t&&) {
    return unique_range<decltype(make_range(r))>(
        make_range(std::forward<T>(r)));
  }
};

inline unique_t unique() { return unique_t(); }

template <typename Range>
struct is_range<unique_range<Range>> : std::true_type {};

}  // namespace utl