#pragma once

#include <algorithm>

namespace utl {

template <typename Container>
Container& concat(Container& c1, Container const& c2) {
  c1.insert(end(c1), begin(c2), end(c2));
  return c1;
}

}  // namespace utl
