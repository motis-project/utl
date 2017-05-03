#pragma once

namespace utl {

template <typename Map, typename K>
size_t get_or_create_index(Map& m, K const& key) {
  auto it = m.find(key);
  if (it != end(m)) {
    return it->second;
  } else {
    return m[key] = m.size();
  }
}

}  // namespace utl
