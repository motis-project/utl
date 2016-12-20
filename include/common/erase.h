#pragma once

#include <algorithm>

namespace common {

template <typename Container, typename Element>
void erase(Container& c, Element elem) {
  c.erase(std::remove(begin(c), end(c), elem), end(c));
}

}  // namespace common
