#pragma once

#include <algorithm>

namespace utl {

template <typename Container1, typename Container2>
Container1& concat(Container1& c1, Container2 const& c2) {
  c1.insert(end(c1), begin(c2), end(c2));
  return c1;
}

}  // namespace utl
