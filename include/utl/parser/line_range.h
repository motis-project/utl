#pragma once

#include "utl/pipes/is_range.h"

namespace utl {

template <typename Reader>
struct line_range {
  line_range(Reader&& r) : r_(std::forward<Reader>(r)) {}

  auto begin() { return r_.read_line(); }

  template <typename It>
  auto&& read(It& it) {
    return it;
  }

  template <typename It>
  void next(It& it) {
    it = r_.read_line();
  }

  template <typename It>
  bool valid(It& it) {
    return it;
  }

  Reader r_;
};

template <typename Reader>
line_range(Reader &&) -> line_range<Reader>;

template <typename Reader>
struct is_range<line_range<Reader>> : std::true_type {};

}  // namespace utl