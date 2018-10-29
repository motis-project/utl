#pragma once

#include <type_traits>

namespace utl {

template <typename T>
using clear_t = std::remove_const_t<std::remove_reference_t<T>>;

}  // namespace utl