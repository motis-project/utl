#pragma once

#include <algorithm>

namespace utl {

template <
    typename Container,
    typename Less = std::less<decltype(*std::declval<Container>().begin())>,
    typename Eq = std::equal_to<decltype(*std::declval<Container>().begin())>>
void erase_duplicates(Container& c, Less&& less = Less{}, Eq&& eq = Eq{}) {
  std::sort(begin(c), end(c), less);
  c.erase(std::unique(begin(c), end(c), eq), end(c));
}

template <typename Container, typename Iterator, typename Less, typename Eq>
void erase_duplicates(Container& c, Iterator const begin, Iterator const end,
                      Less&& less, Eq&& eq) {
  std::sort(begin, end, less);
  c.erase(std::unique(begin, end, eq), end);
}

}  // namespace utl
