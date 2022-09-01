#pragma once

#include <concepts>
#include <iterator>

namespace utl {

template <typename It1, typename It2, typename Predicate>
bool is_uniform(It1&& it1, It2&& it2, Predicate&& fn) {
  auto& pred = *it1;
  for (auto it = ++it1; it != it2; ++it) {
    if (fn(*it) != fn(pred)) {
      return false;
    }
    pred = *it;
  }
  return true;
}

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

template <typename Collection, typename Predicate>
std::enable_if_t<std::is_function_v<Predicate>, bool> is_uniform(
    Collection&& c, Predicate&& fn) {
  using std::begin;
  using std::end;
  return is_uniform(begin(c), end(c), std::forward<Predicate&&>(fn));
}

}  // namespace utl
