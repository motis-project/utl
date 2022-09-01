#pragma once

#include "utl/nwise.h"

namespace utl {

template <typename Container>
auto pairwise(Container&& c) {
  return nwise<2>(std::forward<Container&&>(c));
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
