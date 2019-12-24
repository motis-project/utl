#pragma once

// (heavily) inspired by https://stackoverflow.com/a/14829088

#include <array>
#include <iterator>
#include <utility>

namespace utl {
namespace detail {

template <typename Iterator>
struct pairwise_iterator {
  using ref_t = typename std::iterator_traits<Iterator>::reference;

  pairwise_iterator(Iterator begin, Iterator end)
      : first_(begin), second_(begin == end ? begin : std::next(begin)) {}

  bool operator==(pairwise_iterator const& o) const {
    return second_ == o.second_;
  }

  bool operator!=(pairwise_iterator const& o) const {
    return second_ != o.second_;
  }

  pairwise_iterator& operator++() {
    ++first_;
    ++second_;

    return *this;
  }

  std::pair<ref_t, ref_t> operator*() const {
    return std::pair<ref_t, ref_t>{*first_, *second_};
  }

  Iterator first_, second_;
};

template <typename Iterator>
struct pairwise_range {
  pairwise_range(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

  pairwise_iterator<Iterator> begin() const {
    return pairwise_iterator<Iterator>{begin_, end_};
  }

  pairwise_iterator<Iterator> end() const {
    return pairwise_iterator<Iterator>{end_, end_};
  }

  Iterator begin_, end_;
};

}  // namespace detail

template <typename Container>
auto pairwise(Container& c) -> detail::pairwise_range<decltype(begin(c))> {
  return detail::pairwise_range<decltype(begin(c))>{c.begin(), c.end()};
}

template <typename Container>
auto pairwise_begin(Container& c)
    -> detail::pairwise_iterator<decltype(begin(c))> {
  return detail::pairwise_iterator<decltype(begin(c))>{begin(c), end(c)};
}

template <typename Container>
auto pairwise_end(Container& c)
    -> detail::pairwise_iterator<decltype(begin(c))> {
  return detail::pairwise_iterator<decltype(begin(c))>{end(c), end(c)};
}

}  // namespace utl
