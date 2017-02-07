#pragma once

#include <map>

namespace utl {

template <typename Map, typename K, typename CreateFun>
auto get_or_create(Map& m, K const& key, CreateFun&& f) ->
    typename Map::mapped_type& {
  auto const it = m.find(key);
  return it != end(m) ? it->second : m[key] = f();
}

}  // namespace utl
