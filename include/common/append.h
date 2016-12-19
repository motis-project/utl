#pragma once

#include <algorithm>

namespace common {

template <typename Container>
void append(Container& c1, Container const& c2) {
  c1.insert(end(c1), begin(c2), end(c2));
}

}  // namespace common
