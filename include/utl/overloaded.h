#pragma once

#include <type_traits>

#include "utl/argument_helper.h"

namespace utl {

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

}  // namespace utl
