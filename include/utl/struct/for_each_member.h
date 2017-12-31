#include <type_traits>
#include <utility>

#include "utl/struct/arity.h"
#include "utl/struct/to_tuple.h"

namespace utl {

#ifdef _MSC_VER
template <size_t Index, typename Fn, typename... Args>
void call_for_each(Fn&& fn, std::tuple<Args...>&& t) {
  fn(std::get<Index>(t));
  if constexpr (Index != std::tuple_size_v<std::tuple<Args...>> - 1) {
    call_for_each<Index + 1>(std::forward<Fn>(fn),
                             std::forward<std::tuple<Args...>>(t));
  }
}

template <typename Fn, typename... Args>
void call_for_each(Fn&& fn, std::tuple<Args...>&& t) {
  call_for_each<0, Fn, Args...>(std::forward<Fn>(fn),
                                std::forward<std::tuple<Args...>>(t));
}
#endif

template <typename T, typename Fn>
inline void for_each_field(T& t, Fn&& fn) {
  if constexpr (std::is_pointer_v<T>) {
    if (t != nullptr) {
      for_each_field(*t, std::forward<Fn>(fn));
    }
  } else if constexpr (std::is_scalar_v<T>) {
    f(t);
  } else {
#ifdef _MSC_VER
    call_for_each(std::forward<Fn>(fn), to_tuple(t));
#else
    std::apply([&](auto&&... args) { (fn(args), ...); }, to_tuple(t));
#endif
  }
}

}  // namespace utl