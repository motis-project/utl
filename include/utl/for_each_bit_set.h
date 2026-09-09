#pragma once

#include <bit>
#include <type_traits>

namespace utl {

template <typename T>
constexpr bool has_bit_set(T const levels, unsigned const bit) {
  return (levels & (T{1U} << bit)) != 0U;
}

template <typename T, typename Fn>
constexpr void for_each_set_bit(T const levels, Fn&& f) {
  static_assert(
      std::is_integral_v<T> && !std::is_same_v<std::remove_cv_t<T>, bool>,
      "for_each_set_bit: integral (non-bool) types only");
  auto block = static_cast<std::make_unsigned_t<T>>(levels);
  while (block != 0U) {
    f(static_cast<unsigned>(std::countr_zero(block)));
    block &= static_cast<decltype(block)>(block - 1U);
  }
}

}  // namespace utl
