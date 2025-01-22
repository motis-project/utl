#pragma once

#include <algorithm>
#include <iterator>
#include <vector>

namespace utl {

template <typename Output, typename Container, typename UnaryOperation>
inline Output& transform_to(Container&& c, Output& out, UnaryOperation&& op) {
  using std::begin;
  using std::end;
  out.reserve(static_cast<std::size_t>(std::distance(begin(c), end(c))));
  std::transform(begin(c), end(c), std::back_inserter(out),
                 std::forward<UnaryOperation>(op));
  return out;
}

template <typename Output, typename Container, typename UnaryOperation>
inline auto transform_to(Container&& c, UnaryOperation&& op) -> Output {
  using std::begin;
  using std::end;
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
    -> std::vector<std::decay_t<decltype(op(*std::begin(c)))>> {
  using std::begin;
  using std::end;
  auto v = std::vector<std::decay_t<decltype(op(*std::begin(c)))>>{};
  v.reserve(static_cast<std::size_t>(std::distance(begin(c), end(c))));
  std::transform(begin(c), end(c), std::back_inserter(v),
                 std::forward<UnaryOperation>(op));
  return v;
}

template <typename Container>
inline auto to_vec(Container&& c)
    -> std::vector<std::decay_t<decltype(*std::begin(c))>> {
  using std::begin;
  using std::end;
  auto v = std::vector<std::decay_t<decltype(*begin(c))>>{};
  v.reserve(static_cast<std::size_t>(std::distance(begin(c), end(c))));
  std::copy(begin(c), end(c), std::back_inserter(v));
  return v;
}

}  // namespace utl
