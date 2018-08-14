#pragma once

#include "utl/pipes/all.h"
#include "utl/pipes/is_range.h"

namespace utl {

template <typename T>
auto make_range(T&& t) {
  if constexpr (is_range<
                    std::remove_const_t<std::remove_reference_t<T>>>::value) {
    return t;
  } else {
    return all(std::forward<T>(t));
  }
}

}  // namespace utl