#pragma once

#include <algorithm>

namespace utl {

template <typename Container>
void erase_duplicates(Container& c) {
  std::sort(begin(c), end(c));
  c.erase(std::unique(begin(c), end(c)), end(c));
}

}  // namespace utl
