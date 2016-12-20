#pragma once

#include <algorithm>

namespace utl {

template <typename Container>
void concat(Container& c1, Container const& c2) {
  c1.insert(end(c1), begin(c2), end(c2));
}

}  // namespace utl
