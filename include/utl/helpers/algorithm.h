#pragma once

#include <algorithm>
#include <iterator>

namespace utl {

template <typename Collection, typename Predicate>
constexpr bool all_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::all_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
constexpr bool any_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::any_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
constexpr bool none_of(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::none_of(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
constexpr auto find_if(Collection&& c, Predicate&& p) {
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

template <typename Collection, typename Predicate>
constexpr void sort(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  std::sort(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename Predicate>
constexpr bool is_sorted(Collection&& c, Predicate&& p) {
  using std::begin;
  using std::end;
  return std::is_sorted(begin(c), end(c), std::forward<Predicate>(p));
}

template <typename Collection, typename T>
constexpr void fill(Collection&& c, T const& value) {
  std::fill(begin(c), end(c), value);
}

}  // namespace utl