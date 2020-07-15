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
struct holding_range {
  using result_t =
      clear_t<decltype(*std::declval<typename Container::const_iterator>())>;
  using it_t = typename Container::const_iterator;
  using begin_t = typename Container::const_iterator;
  using end_t = typename Container::const_iterator;

  explicit holding_range(Container&& c) : c_{std::forward<decltype(c)>(c)} {}

  auto begin() const { return c_.begin(); }
  auto end() const { return c_.end(); }

  template <typename It,
            std::enable_if_t<std::is_reference_v<decltype(*std::declval<It>())>,
                             int> = 0>
  auto&& read(It& it) const {
    return *it;
  }

  template <typename It,
            std::enable_if_t<
                !std::is_reference_v<decltype(*std::declval<It>())>, int> = 0>
  auto read(It& it) const {
    return *it;
  }

  template <typename It>
  void next(It& it) const {
    ++it;
  }

  template <typename It>
  bool valid(It& it) const {
    return it != end();
  }

  std::decay_t<Container> c_;
};

template <typename Container>
holding_range(Container)->holding_range<std::decay_t<Container>>;

template <typename Container>
struct is_range<holding_range<Container>> : std::true_type {};

template <typename Container>
auto all(Container&& c) {
  return holding_range<Container>{std::forward<Container>(c)};
}

template <typename T>
auto all(std::initializer_list<T>&& c) {
  return holding_range{std::forward<decltype(c)>(c)};
}

}  // namespace utl