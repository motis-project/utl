#pragma once

namespace utl {

template <typename T>
constexpr bool has_bit_set(T const levels, unsigned const bit) {
  return (levels & (T{1U} << bit)) != 0U;
}

template <typename T, typename Fn>
constexpr void for_each_set_bit(T const levels, Fn&& f) {
  for (auto bit = 0U; bit != sizeof(T) * 8U; ++bit) {
    if (has_bit_set(levels, bit)) {
      f(bit);
    }
  }
}

}  // namespace utl