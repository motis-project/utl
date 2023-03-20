#pragma once

#include <algorithm>
#include <iterator>

namespace utl {

template <typename Collection, typename Predicate>
bool all_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::all_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
bool any_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::any_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
bool none_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::none_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
auto find_if(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::find_if(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
auto count_if(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::count_if(begin(c), end(c), std::forward<Predicate>(p));
}

}  // namespace utl