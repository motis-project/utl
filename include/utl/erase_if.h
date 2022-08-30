#pragma once

#include <algorithm>

namespace utl {

template <typename Container, typename Predicate>
void erase_if(Container&& c, Predicate&& pred) {
  c.erase(std::remove_if(begin(c), end(c), std::forward<Predicate>(pred)),
          end(c));
}

}  // namespace utl
