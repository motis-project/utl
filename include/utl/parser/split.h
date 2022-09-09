#pragma once

#include "utl/parser/csv.h"

namespace utl {

template <char Separator = ',', typename... Ts>
std::tuple<Ts...> split(cstr s) {
  std::tuple<Ts...> ret;
  auto const parse_col =
      [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
        auto col = cstr{};
        parse_column<cstr, Separator>(s, col);
        parse_arg(col, std::get<I>(ret));
        if (s) {
          ++s;
        }
      };
  auto const parse_tuple = [&]<std::size_t... I>(std::index_sequence<I...>) {
    (parse_col(std::integral_constant<std::size_t, I>{}), ...);
  };
  parse_tuple(std::make_index_sequence<sizeof...(Ts)>());
  return ret;
}

}  // namespace utl