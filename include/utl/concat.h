#pragma once

#include <algorithm>

namespace utl {

template <typename Container1, typename Container2>
Container1& concat(Container1& c1, Container2 const& c2) {
  c1.insert(end(c1), begin(c2), end(c2));
  return c1;
}

template <typename Container1, typename Container2>
Container1 merge(Container1 const& c1, Container2 const& c2) {
  if (c2.empty()) {
    return c1;
  }
  auto c = c1;
  c.insert(end(c), begin(c2), end(c2));
  return c;
}

}  // namespace utl
