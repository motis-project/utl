#pragma once

#include <algorithm>
#include <atomic>
#include <iostream>
#include <thread>

#include "utl/logging.h"

namespace utl {

template <typename Fun>
inline void parallel_for(size_t const job_count, Fun func) {
  std::atomic<size_t> counter(0);
  std::vector<std::thread> threads;
  for (auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
    threads.emplace_back([&]() {
      while (true) {
        auto const idx = counter.fetch_add(1);
        if (idx >= job_count) {
          break;
        }

        func(idx);
      }
    });
  }

  std::for_each(begin(threads), end(threads), [](auto& t) { t.join(); });
}

template <typename T, typename Fun>
inline void parallel_for(std::vector<T> const& jobs, Fun func) {
  parallel_for(jobs.size(), [&](auto const idx) { func(jobs[idx]); });
}

template <typename T, typename Fun>
inline void parallel_for(std::string const& desc, std::vector<T> const& jobs,
                         size_t const mod, Fun func) {
  parallel_for(jobs.size(), [&](auto const idx) {
    if (idx % mod == 0) {
      uLOG(info) << desc << " " << idx << "/" << jobs.size();
    }
    func(jobs[idx]);
  });
}

}  // namespace utl
