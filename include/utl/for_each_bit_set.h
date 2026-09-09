#pragma once

#include <bit>
#include <type_traits>

namespace utl {

template <typename T>
constexpr bool has_bit_set(T const levels, unsigned const bit) {
  return (levels & (T{1U} << bit)) != 0U;
}

// Calls `f(bit)` for every set bit of `levels`, lowest first.
//
// Only the set bits are visited: `countr_zero` jumps to the next one and
// `rest & (rest - 1)` clears it, so the number of iterations is the population
// count rather than the width of `T`. That also drops the per bit branch,
// which is why this stays ahead even when every bit is set.
template <typename T, typename Fn>
constexpr void for_each_set_bit(T const levels, Fn&& f) {
  static_assert(
      std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>,
      "for_each_set_bit: integral (non-bool) types only");
  auto rest = static_cast<std::make_unsigned_t<T>>(levels);
  while (rest != 0U) {
    f(static_cast<unsigned>(std::countr_zero(rest)));
    rest &= static_cast<decltype(rest)>(rest - 1U);
  }
}

}  // namespace utl
