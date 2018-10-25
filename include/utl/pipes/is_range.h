#pragma once

#include <type_traits>

namespace utl {

template <typename... Args>
struct is_range : std::false_type {};

}  // namespace utl