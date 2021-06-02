#pragma once

#include <stdexcept>
#include <type_traits>

#include "utl/overloaded.h"

namespace utl {

template <typename Parent, typename Fn>
auto match(Parent&& p, Fn&& f) {
  if (auto const casted = dynamic_cast<first_argument<Fn>>(p);
      casted != nullptr) {
    return f(casted);
  } else {
    throw std::runtime_error{"no matching class found"};
  }
}

template <typename Parent, typename Fn, typename... RestFn>
auto match(Parent&& p, Fn&& f, RestFn&&... rest_f) {
  if (auto const casted = dynamic_cast<first_argument<Fn>>(p);
      casted != nullptr) {
    return f(casted);
  } else {
    return match(std::forward<Parent>(p), std::forward<RestFn>(rest_f)...);
  }
}

}  // namespace utl
