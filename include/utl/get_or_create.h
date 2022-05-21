#pragma once

#include <map>

namespace utl {

namespace detail {

template <typename T, typename = void>
struct help_has_emplace_hint : std::false_type {};

template <typename T>
struct help_has_emplace_hint<
    T, std::void_t<decltype(std::declval<T>().emplace_hint(
           std::declval<typename T::iterator>(),
           std::declval<typename T::key_type>(),
           std::declval<typename T::mapped_type>()))>> : std::true_type {};

template <typename T>
inline constexpr auto const has_emplace_hint = help_has_emplace_hint<T>::value;

}  // namespace detail

template <typename Map, typename K, typename CreateFun>
auto get_or_create(Map& m, K&& key, CreateFun&& f) ->
    typename Map::mapped_type& {
  if (auto const it = m.find(key); it == end(m)) {
    if constexpr (detail::has_emplace_hint<Map>) {
      return m
          .emplace_hint(it, typename Map::key_type{std::forward<K>(key)}, f())
          ->second;
    } else {
      return m.emplace(typename Map::key_type{std::forward<K>(key)}, f())
          .first->second;
    }
  } else {
    return it->second;
  }
}

}  // namespace utl
