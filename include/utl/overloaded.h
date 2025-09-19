#pragma once

namespace utl {

template <typename Ret, typename Arg, typename... Rest>
Arg first_argument_helper(Ret (*)(Arg, Rest...));

template <typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret (F::*)(Arg, Rest...));

template <typename Ret, typename F, typename Arg, typename... Rest>
Arg first_argument_helper(Ret (F::*)(Arg, Rest...) const);

template <typename F>
decltype(first_argument_helper(&F::operator())) first_argument_helper(F);

template <typename T>
using first_argument = decltype(first_argument_helper(std::declval<T>()));

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;

  template <typename T>
  std::enable_if_t<
      (!std::is_same_v<std::decay_t<T>, std::decay_t<first_argument<Ts>>> &&
       ...)>
  operator()(T&&) {}
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template <class... Ts, typename Variant>
auto apply(overloaded<Ts...>&& x, Variant&& variant) {
  return std::apply(std::move(x), variant);
}

}  // namespace utl