#pragma once

#include <iterator>

namespace utl {

template <typename It1, typename It2>
bool is_uniform(It1&& it1, It2&& it2) {
  auto& pred = *it1;
  for (auto it = ++it1; it != it2; ++it) {
    if (*it != pred) {
      return false;
    }
    pred = *it;
  }
  return true;
}

template <typename Collection>
bool is_uniform(Collection&& c) {
  using std::begin;
  using std::end;
  return is_uniform(begin(c), end(c));
}

}  // namespace utl