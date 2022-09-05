#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace utl {

template <typename Output, typename Container, typename UnaryOperation>
inline void transform_to(Container&& c, Output& out, UnaryOperation&& op) {
  out.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::transform(std::begin(c), std::end(c), std::back_inserter(out),
                 std::forward<UnaryOperation>(op));
}

template <typename Output, typename Container, typename UnaryOperation>
inline auto transform_to(Container&& c, UnaryOperation&& op) -> Output {
  Output v;
  v.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::transform(std::begin(c), std::end(c), std::back_inserter(v),
                 std::forward<UnaryOperation>(op));
  return v;
}

template <typename It, typename UnaryOperation>
inline auto to_vec(It s, It e, UnaryOperation&& op)
    -> std::vector<decltype(op(*s))> {
  std::vector<decltype(op(*s))> v;
  v.reserve(static_cast<std::size_t>(std::distance(s, e)));
  std::transform(s, e, std::back_inserter(v), std::forward<UnaryOperation>(op));
  return v;
}

template <typename Container, typename UnaryOperation>
inline auto to_vec(Container&& c, UnaryOperation&& op)
    -> std::vector<decltype(op(*std::begin(c)))> {
  std::vector<decltype(op(*std::begin(c)))> v;
  v.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::transform(std::begin(c), std::end(c), std::back_inserter(v),
                 std::forward<UnaryOperation>(op));
  return v;
}

template <typename Container>
inline auto to_vec(Container&& c) -> std::vector<decltype(*std::begin(c))> {
  std::vector<decltype(*std::begin(c))> v;
  v.reserve(
      static_cast<std::size_t>(std::distance(std::begin(c), std::end(c))));
  std::copy(std::begin(c), std::end(c), std::back_inserter(v));
  return v;
}

}  // namespace utl
