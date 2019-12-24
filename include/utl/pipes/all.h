#pragma once

#include <initializer_list>
#include <utility>

#include "utl/clear_t.h"
#include "utl/pipes/is_range.h"
#include "utl/pipes/range.h"

namespace utl {

template <typename BeginIt, typename EndIt>
struct is_range<range<BeginIt, EndIt>> : std::true_type {};

template <typename Container>
auto all(Container& c) {
  using std::begin;
  using std::end;
  return range<decltype(begin(c)), decltype(end(c))>{begin(c), end(c)};
}

template <typename BeginIt, typename EndIt>
auto all(BeginIt&& a, EndIt&& e) {
  return range<BeginIt, EndIt>{std::forward<BeginIt>(a),
                               std::forward<EndIt>(e)};
}

template <typename Container>
struct holding_range : public range<typename Container::const_iterator,
                                    typename Container::const_iterator> {
  using parent = range<typename Container::const_iterator,
                       typename Container::const_iterator>;

  explicit holding_range(Container&& c)
      : parent{begin(c), end(c)}, c_{std::forward<Container>(c)} {}

  Container c_;
};

template <typename Container>
struct is_range<holding_range<Container>> : std::true_type {};

template <typename Container>
auto all(Container&& c) {
  return holding_range<Container>{std::forward<Container>(c)};
}

template <typename T>
auto all(std::initializer_list<T>&& c) {
  return range<decltype(begin(c)), decltype(end(c))>{begin(c), end(c)};
}

}  // namespace utl