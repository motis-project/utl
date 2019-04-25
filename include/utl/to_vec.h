#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace utl {

template <typename It, typename UnaryOperation>
inline auto to_vec(It s, It e, UnaryOperation&& op)
    -> std::vector<decltype(op(*s))> {
  std::vector<decltype(op(*s))> v;
  v.reserve(static_cast<std::size_t>(std::distance(s, e)));
  std::transform(s, e, std::back_inserter(v), op);
  return v;
}

template <typename Container, typename UnaryOperation>
inline auto to_vec(Container const& c, UnaryOperation&& op)
    -> std::vector<decltype(op(*std::begin(c)))> {
  std::vector<decltype(op(*std::begin(c)))> v;
  v.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::transform(std::begin(c), std::end(c), std::back_inserter(v), op);
  return v;
}

template <typename Container>
inline auto to_vec(Container const& c)
    -> std::vector<decltype(*std::begin(c))> {
  std::vector<decltype(*std::begin(c))> v;
  v.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::copy(std::begin(c), std::end(c), std::back_inserter(v));
  return v;
}

}  // namespace utl
