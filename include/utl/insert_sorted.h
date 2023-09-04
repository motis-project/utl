#pragma once

#include <algorithm>
#include <utility>

namespace utl {

template <typename Collection, typename Less>
std::pair<typename std::decay_t<Collection>::iterator, bool> insert_sorted(
    Collection&& v, typename std::decay_t<Collection>::value_type el,
    Less&& less) {
  using std::begin;
  using std::end;
  auto const it =
      std::lower_bound(begin(v), end(v), el, std::forward<Less>(less));
  if (it == std::end(v) || *it != el) {
    return {v.insert(it, std::move(el)), true};
  }
  return {it, false};
}

template <typename Collection>
std::pair<typename std::decay_t<Collection>::iterator, bool> insert_sorted(
    Collection&& v, typename std::decay_t<Collection>::value_type el) {
  return insert_sorted(
      std::forward<decltype(v)>(v), std::move(el),
      std::less<typename std::decay_t<Collection>::value_type>{});
}

}  // namespace utl