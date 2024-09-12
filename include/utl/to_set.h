#pragma once

#include <algorithm>
#include <iterator>
#include <set>

namespace utl {

template <typename It, typename UnaryOperation>
inline auto to_set(It b, It e,
                   UnaryOperation&& op) -> std::set<decltype(op(*b))> {
  using set = std::set<decltype(op(*b))>;
  set s(std::distance(s, e));
  std::transform(b, e, std::insert_iterator<set>(s), op);
  return s;
}

template <typename Container, typename UnaryOperation>
inline auto to_set(Container const& c, UnaryOperation&& op)
    -> std::set<decltype(op(*std::begin(c)))> {
  using set = std::set<decltype(op(*std::begin(c)))>;
  set s;
  std::transform(std::begin(c), std::end(c),
                 std::insert_iterator<set>(s, begin(s)), op);
  return s;
}

}  // namespace utl
