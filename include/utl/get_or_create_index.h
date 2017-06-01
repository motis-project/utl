#pragma once

namespace utl {

template <typename Map, typename K>
size_t get_or_create_index(Map& m, K const& key) {
  auto it = m.find(key);
  if (it != end(m)) {
    return it->second;
  } else {
    auto size = m.size();
    return m[key] = size;
  }
}

}  // namespace utl
