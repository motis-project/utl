#pragma once

#include <algorithm>

namespace utl {

struct DefaultLess {
  template <typename T>
  bool operator()(const T& a, const T& b) const {
    return a < b;
  }
};

struct DefaultEq {
  template <typename T>
  bool operator()(const T& a, const T& b) const {
    return a == b;
  }
};

template <typename Container, typename Less = DefaultLess,
          typename Eq = DefaultEq>
void erase_duplicates(Container& c, Less&& less = Less{}, Eq&& eq = Eq{}) {
  std::sort(begin(c), end(c), std::forward<Less>(less));
  c.erase(std::unique(begin(c), end(c), std::forward<Eq>(eq)), end(c));
}

template <typename Container, typename Iterator, typename Less, typename Eq>
void erase_duplicates(Container& c, Iterator const begin, Iterator const end,
                      Less&& less, Eq&& eq) {
  std::sort(begin, end, less);
  c.erase(std::unique(begin, end, eq), end);
}

}  // namespace utl
