#pragma once

#include <algorithm>
#include <atomic>
#include <exception>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "utl/logging.h"

namespace utl {

enum class parallel_error_strategy { CONTINUE_EXEC, QUIT_EXEC };

using errors_t = std::vector<std::pair<size_t, std::exception_ptr>>;

struct noop_progress_update {
  void operator()(size_t const) {}
};

template <typename ThreadLocal, typename Fun,
          typename ProgressUpdateFn = noop_progress_update>
inline errors_t parallel_for_run_threadlocal(
    size_t const job_count, Fun func,
    ProgressUpdateFn&& progress_update = ProgressUpdateFn{},
    parallel_error_strategy const err_strat =
        parallel_error_strategy::QUIT_EXEC) {
  errors_t errors;
  std::mutex errors_mutex;
  std::atomic<size_t> counter(0);
  std::atomic<bool> quit{false};
  std::vector<std::thread> threads;
  for (auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
    threads.emplace_back([&]() {
      auto threadlocal = ThreadLocal{};

      while (!quit) {
        auto const idx = counter.fetch_add(1);
        if (idx >= job_count) {
          break;
        }

        try {
          func(threadlocal, idx);
          progress_update(idx);
        } catch (...) {
          std::lock_guard<std::mutex> lock{errors_mutex};
          errors.emplace_back(std::pair{i, std::current_exception()});
          if (err_strat == parallel_error_strategy::QUIT_EXEC) {
            quit = true;
            break;
          }
        }
      }
    });
  }

  std::for_each(begin(threads), end(threads), [](auto& t) { t.join(); });

  if (err_strat == parallel_error_strategy::QUIT_EXEC && !errors.empty()) {
    std::rethrow_exception(errors.front().second);
  }

  return errors;
}

template <typename Fun, typename ProgressUpdateFn = noop_progress_update>
inline errors_t parallel_for_run(
    size_t const job_count, Fun func,
    ProgressUpdateFn&& progress_update = ProgressUpdateFn{},
    parallel_error_strategy const err_strat =
        parallel_error_strategy::QUIT_EXEC) {
  errors_t errors;
  std::mutex errors_mutex;
  std::atomic<size_t> counter(0);
  std::atomic<bool> quit{false};
  std::vector<std::thread> threads;
  for (auto i = 0u; i < std::thread::hardware_concurrency(); ++i) {
    threads.emplace_back([&]() {
      while (!quit) {
        auto const idx = counter.fetch_add(1);
        if (idx >= job_count) {
          break;
        }

        try {
          func(idx);
          progress_update(idx);
        } catch (...) {
          std::lock_guard<std::mutex> lock{errors_mutex};
          errors.emplace_back(std::pair{i, std::current_exception()});
          if (err_strat == parallel_error_strategy::QUIT_EXEC) {
            quit = true;
            break;
          }
        }
      }
    });
  }

  std::for_each(begin(threads), end(threads), [](auto& t) { t.join(); });

  if (err_strat == parallel_error_strategy::QUIT_EXEC && !errors.empty()) {
    std::rethrow_exception(errors.front().second);
  }

  return errors;
}

template <typename Container, typename Fun,
          typename ProgressUpdateFn = noop_progress_update>
inline errors_t parallel_for(
    Container& jobs, Fun&& func,
    ProgressUpdateFn&& progress_update = ProgressUpdateFn{},
    parallel_error_strategy const err_strat =
        parallel_error_strategy::QUIT_EXEC) {
  return parallel_for_run(
      jobs.size(),
      [&](auto const idx) {
        {
          using std::begin;
          func(*std::next(begin(jobs),
                          static_cast<typename std::iterator_traits<
                              decltype(begin(jobs))>::difference_type>(idx)));
        }
      },
      std::forward<ProgressUpdateFn>(progress_update), err_strat);
}

template <typename Container, typename Fun,
          typename ProgressUpdateFn = noop_progress_update>
inline errors_t parallel_for(
    Container const& jobs, Fun&& func,
    ProgressUpdateFn&& progress_update = ProgressUpdateFn{},
    parallel_error_strategy const err_strat =
        parallel_error_strategy::QUIT_EXEC) {
  return parallel_for_run(
      jobs.size(),
      [&](auto const idx) {
        {
          using std::begin;
          func(*std::next(begin(jobs),
                          static_cast<typename std::iterator_traits<
                              decltype(begin(jobs))>::difference_type>(idx)));
        }
      },
      std::forward<ProgressUpdateFn>(progress_update), err_strat);
}

template <typename Container, typename Fun,
          typename ProgressUpdateFn = noop_progress_update>
inline errors_t parallel_for(
    std::string const& desc, Container const& jobs, size_t const mod, Fun func,
    ProgressUpdateFn&& progress_update = ProgressUpdateFn{},
    parallel_error_strategy const err_strat =
        parallel_error_strategy::QUIT_EXEC) {
  return parallel_for_run(
      jobs.size(),
      [&](auto const idx) {
        if (idx % mod == 0) {
          utl::log_info("parallel_for", "{} {}/{}", desc, idx, jobs.size());
        }
        func(jobs[idx]);
      },
      std::forward<ProgressUpdateFn>(progress_update), err_strat);
}

}  // namespace utl
