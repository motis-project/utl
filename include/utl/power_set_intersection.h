#pragma once

#include <functional>
#include <type_traits>

namespace utl {

template <bool Swap, typename It, typename Lt, typename Fn>
void for_each_equal(It const a, It b, It const b_end, Lt&& lt, Fn&& fn) {
  while (b != b_end) {
    if (lt(*a, *b) || lt(*b, *a)) {
      return;
    }
    Swap ? fn(*b, *a) : fn(*a, *b);
    ++b;
  }
}

template <typename It, typename Fn,
          typename Lt = std::less<decltype(*std::declval<It>())>>
void power_set_intersection(It const a_begin, It const a_end, It const b_begin,
                            It const b_end, Fn&& fn, Lt&& lt = Lt{}) {
  auto a = a_begin;
  auto b = b_begin;
  while (a != a_end && b != b_end) {
    if (lt(*a, *b)) {
      ++a;
    } else if (lt(*b, *a)) {
      ++b;
    } else {
      for_each_equal<false>(a, b, b_end, lt, fn);
      ++a;
      for_each_equal<true>(b, a, a_end, lt, fn);
      ++b;
    }
  }
}

template <typename A, typename B, typename Fn,
          typename Lt = std::less<decltype(*begin(std::declval<A>()))>>
void power_set_intersection(A const& a, B const& b, Fn&& fn, Lt&& lt = Lt{}) {
  power_set_intersection(begin(a), end(a), begin(b), end(b),
                         std::forward<Fn>(fn), std::forward<Lt>(lt));
}

}  // namespace utl