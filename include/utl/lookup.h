#pragma once

#include <optional>

namespace utl {

template <typename Map, typename Key>
auto lookup(Map const& m, Key const& key) {
  auto const it = m.find(key);
  return it == end(m) ? std::nullopt : std::optional{it->second};
}

}  // namespace utl
