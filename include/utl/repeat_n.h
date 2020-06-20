#pragma once

#include <algorithm>
#include <vector>

namespace utl {

// TODO replace this in motis::loader
template <typename T, typename Container = std::vector<T>>
Container repeat_n(T const& el, size_t const n) {
  Container els(n);
  std::fill(begin(els), end(els), el);
  return els;
}

}  // namespace utl
