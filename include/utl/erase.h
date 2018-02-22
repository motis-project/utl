#pragma once

#include <algorithm>
#include <set>

namespace utl {

template <typename Container, typename Element>
void erase(Container& c, Element elem) {
  c.erase(std::remove(begin(c), end(c), elem), end(c));
}

template <typename T, typename Element>
void erase(std::set<T>& c, Element elem) {
  c.erase(elem);
}

}  // namespace utl
