#pragma once

#include <algorithm>

namespace utl {

template <typename Container, typename F>
void equal_ranges(Container& c, F&& func) {
  std::sort(begin(c), end(c));
  auto lower = begin(c);
  while (lower != end(c)) {
    auto upper = std::upper_bound(begin(c), end(c), *lower);
    func(lower, upper);
    lower = upper;
  }
}


template <typename Container, typename Cmp, typename F>
void equal_ranges(Container& c, Cmp&& cmp, F&& func) {
  std::sort(begin(c), end(c), cmp);
  auto lower = begin(c);
  while (lower != end(c)) {
    auto upper = std::upper_bound(begin(c), end(c), *lower, cmp);
    func(lower, upper);
    lower = upper;
  }
}

}  // namespace utl
