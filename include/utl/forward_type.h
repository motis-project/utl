#pragma once

#include <type_traits>

namespace utl {

namespace detail {

template <typename T, typename Enable = void>
struct forward_type;

template <typename T>
struct forward_type<T, std::enable_if_t<std::is_lvalue_reference_v<T>>> {
  using value_t = T;
};

template <typename T>
struct forward_type<T, std::enable_if_t<std::is_rvalue_reference_v<T>>> {
  using value_t = std::remove_reference_t<T>;
};

template <typename T>
struct forward_type<T, std::enable_if_t<!std::is_reference_v<T>>> {
  using value_t = T;
};

}  // namespace detail

template <typename T>
using forward_type_t = typename detail::forward_type<T>::value_t;

}  // namespace utl