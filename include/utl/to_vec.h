#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace utl {

template <typename Output, typename Container, typename UnaryOperation>
inline void transform_to(Container&& c, Output& out, UnaryOperation&& op) {
  out.reserve(static_cast<std::size_t>(std::distance(begin(c), end(c))));
  std::transform(begin(c), end(c), std::back_inserter(out),
                 std::forward<UnaryOperation>(op));
}

template <typename Output, typename Container, typename UnaryOperation>
inline auto transform_to(Container&& c, UnaryOperation&& op) -> Output {
  Output v;
  v.reserve(static_cast<std::size_t>(std::distance(begin(c), end(c))));
  std::transform(begin(c), end(c), std::back_inserter(v),
                 std::forward<UnaryOperation>(op));
  return v;
}

template <typename It, typename UnaryOperation>
inline auto to_vec(It s, It e, UnaryOperation&& op)
    -> std::vector<std::decay_t<decltype(op(*s))>> {
  auto v = std::vector<std::decay_t<decltype(op(*s))>>{};
  v.reserve(static_cast<std::size_t>(std::distance(s, e)));
  std::transform(s, e, std::back_inserter(v), std::forward<UnaryOperation>(op));
  return v;
}

template <typename Container, typename UnaryOperation>
inline auto to_vec(Container&& c, UnaryOperation&& op)
    -> std::vector<std::decay_t<decltype(op(*begin(c)))>> {
  using std::begin;
  using std::end;
  auto v = std::vector<std::decay_t<decltype(op(*begin(c)))>>{};
  v.reserve(c.size());
  std::transform(begin(c), end(c), std::back_inserter(v),
                 std::forward<UnaryOperation>(op));
  return v;
}

template <typename Container>
inline auto to_vec(Container&& c)
    -> std::vector<std::decay_t<decltype(*begin(c))>> {
  using std::begin;
  using std::end;
  auto v = std::vector<std::decay_t<decltype(*begin(c))>>{};
  v.reserve(c.size());
  std::copy(begin(c), end(c), std::back_inserter(v));
  return v;
}

}  // namespace utl
