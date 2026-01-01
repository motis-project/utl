#pragma once

#include <algorithm>
#include <iterator>

namespace utl {

template <typename Iterator, typename F>
void equal_ranges_linear(Iterator begin, Iterator end, F&& func) {
  auto lower = begin;
  while (lower != end) {
    auto upper = lower;
    while (upper != end && (lower == upper || *lower == *upper)) {
      ++upper;
    }
    func(lower, upper);
    lower = upper;
  }
}

template <typename Container, typename F>
void equal_ranges_linear(Container&& c, F&& func) {
  equal_ranges_linear(std::begin(c), std::end(c), std::forward<F&&>(func));
}

template <typename Iterator, typename Eq, typename F>
void equal_ranges_linear(Iterator begin, Iterator end, Eq&& eq, F&& func) {
  auto lower = begin;
  while (lower != end) {
    auto upper = lower;
    while (upper != end && (lower == upper || eq(*lower, *upper))) {
      ++upper;
    }
    func(lower, upper);
    lower = upper;
  }
}

template <typename Container, typename Eq, typename F>
void equal_ranges_linear(Container&& c, Eq&& eq, F&& func) {
  equal_ranges_linear(std::begin(c), std::end(c), std::forward<Eq&&>(eq),
                      std::forward<F&&>(func));
}

}  // namespace utl
