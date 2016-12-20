#pragma once

#include <map>

namespace utl {

template <typename K, typename V, typename CreateFun>
V& get_or_create(std::map<K, V>& m, K const& key, CreateFun&& f) {
  auto const it = m.find(key);
  return it != end(m) ? it->second : m[key] = f();
}

}  // namespace utl
