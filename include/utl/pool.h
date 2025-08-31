#pragma once

#include <mutex>

namespace utl {

template <typename T>
struct pool {
  T get() {
    auto l = std::scoped_lock{m_};
    if (pool_.empty()) {
      return T{};
    } else {
      auto t = std::move(pool_.back());
      pool_.resize(pool_.size() - 1U);
      return t;
    }
  }

  void put(T&& t) {
    auto l = std::scoped_lock{m_};
    pool_.emplace_back(std::move(t));
  }

  std::mutex m_;
  std::vector<T> pool_;
};

}  // namespace utl