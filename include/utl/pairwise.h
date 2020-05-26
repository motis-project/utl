#pragma once

#include "utl/nwise.h"

namespace utl {

template <typename Container>
auto pairwise(Container& c) -> nwise_range<2, decltype(begin(c))> {
  return {c.begin(), c.end()};
}

template <typename Container>
auto pairwise_begin(Container& c) -> nwise_iterator<2, decltype(begin(c))> {
  return {begin(c), end(c)};
}

template <typename Container>
auto pairwise_end(Container& c) -> nwise_iterator<2, decltype(begin(c))> {
  return {end(c), end(c)};
}

}  // namespace utl
