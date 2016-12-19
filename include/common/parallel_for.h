#pragma once

#include <algorithm>
#include <atomic>
#include <thread>
#include <iostream>

namespace common {

template <typename T, typename Fun>
inline void parallel_for(std::string const& desc, std::vector<T> const& jobs,
                         size_t const mod, Fun func) {
  std::atomic<size_t> counter(0);
  std::vector<std::thread> threads;
  for (auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
    // for (auto i = 0u; i < 1; ++i) {
    threads.emplace_back([&]() {
      while (true) {
        auto const idx = counter.fetch_add(1);
        if (idx >= jobs.size()) {
          break;
        }

        if (idx % mod == 0) {
          std::cout << desc << " " << idx << "/" << jobs.size() << "\n";
        }

        func(jobs[idx]);
      }
    });
  }

  std::for_each(begin(threads), end(threads), [](auto& t) { t.join(); });
}

}  // namespace common
