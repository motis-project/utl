#pragma once

#include <algorithm>

namespace utl {

template <typename Container>
void erase_duplicates(Container& c) {
  std::sort(begin(c), end(c));
  c.erase(std::unique(begin(c), end(c)), end(c));
}

template <typename Container, typename Less, typename Eq>
void erase_duplicates(Container& c, Less&& less, Eq&& eq) {
  std::sort(begin(c), end(c), less);
  c.erase(std::unique(begin(c), end(c), eq), end(c));
}

}  // namespace utl
