#pragma once

#include <type_traits>
#include <variant>

#include "utl/overloaded.h"

namespace utl {

template <class First, class... Rest>
struct visit_overloaded : First, Rest... {
  using First::operator();
  using Rest::operator()...;

  using return_t = std::invoke_result_t<First, first_argument<First>>;

  template <typename T>
  static inline constexpr auto const no_overload =
      !std::is_same_v<std::decay_t<T>, std::decay_t<first_argument<First>>> &&
      (!std::is_same_v<std::decay_t<T>, std::decay_t<first_argument<Rest>>> &&
       ...);

  template <typename T>
  std::enable_if_t<no_overload<T>, return_t> operator()(T&&) {
    if constexpr (std::is_default_constructible_v<return_t>) {
      return {};
    } else {
      static_assert(std::is_same_v<return_t, void>);
      return;
    }
  }
};

template <class... Ts>
visit_overloaded(Ts...) -> visit_overloaded<Ts...>;

template <typename Variant, typename... Fs>
auto visit(Variant&& variant, Fs&&... x) {
  return std::visit(visit_overloaded{std::forward<Fs>(x)...}, variant);
}

}  // namespace utl