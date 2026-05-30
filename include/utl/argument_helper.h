#pragma once

#include <utility>

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

template <typename Ret, typename A0, typename Arg, typename... Rest>
Arg second_argument_helper(Ret (*)(A0, Arg, Rest...));

template <typename Ret, typename F, typename A0, typename Arg, typename... Rest>
Arg second_argument_helper(Ret (F::*)(A0, Arg, Rest...));

template <typename Ret, typename F, typename A0, typename Arg, typename... Rest>
Arg second_argument_helper(Ret (F::*)(A0, Arg, Rest...) const);

template <typename F>
decltype(second_argument_helper(&F::operator())) second_argument_helper(F);

template <typename T>
using second_argument = decltype(second_argument_helper(std::declval<T>()));

}  // namespace utl
