#pragma once

#include <algorithm>
#include <vector>

namespace utl {

// TODO replace this in motis::loader
template <typename T>
std::vector<T> repeat_n(T const& el, size_t const n) {
  std::vector<T> els(n);
  std::fill(begin(els), end(els), el);
  return els;
}

}  // namespace utl
