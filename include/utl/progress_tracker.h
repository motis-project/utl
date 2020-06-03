#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <mutex>
#include <string>

namespace utl {

struct progress_tracker {
  struct txn {
    explicit txn(progress_tracker* tracker)
        : tracker_{tracker}, lock_{tracker_->mutex_} {}
    ~txn();

    txn(txn const&) = delete;
    txn& operator=(txn const&) = delete;
    txn(txn&&) = default;
    txn& operator=(txn&&) = default;

    txn msg(std::string const&);
    txn show_progress(bool);

    txn reset_bounds();
    txn out_bounds(float out_low, float out_high);
    txn out_mod(float out_mod);
    txn in_high(size_t in_high);

    progress_tracker* tracker_;
    std::unique_lock<std::mutex> lock_;
    bool status_changed_{false};
    bool bounds_changed_{false};
  };

  explicit progress_tracker(
      std::function<void(progress_tracker const&)> callback)
      : callback_{std::move(callback)} {}

  txn msg(std::string const&);
  txn show_progress(bool);

  txn reset_bounds();
  txn out_bounds(float out_low, float out_high);
  txn out_mod(float out_mod);
  txn in_high(size_t in_high);

  void update(size_t new_in);
  void increment(size_t inc = 1);
  void update_monotonic(size_t new_in);

  void update_out();

  std::function<void(progress_tracker const&)> callback_;

  mutable std::mutex mutex_;
  std::string msg_;
  bool show_progress_{true};
  float out_low_{0.F};
  float out_high_{100.F};
  float out_mod_{1.F};
  size_t in_high_{100ULL};

  std::atomic_size_t in_{0ULL};
  std::atomic<float> out_{0.F};
};

struct global_progress_trackers {
  global_progress_trackers() = default;
  global_progress_trackers(global_progress_trackers const&) = delete;
  global_progress_trackers& operator=(global_progress_trackers const&) = delete;
  global_progress_trackers(global_progress_trackers&&) = delete;
  global_progress_trackers& operator=(global_progress_trackers&&) = delete;

  progress_tracker& get_tracker(std::string const& name);

  void print();
  void clear();

  std::mutex mutex_;
  bool silent_{false};
  unsigned last_print_height_{0U};
  std::map<std::string, progress_tracker> trackers_;
  progress_tracker* active_tracker_{nullptr};
};

global_progress_trackers& get_global_progress_trackers();

progress_tracker& activate_progress_tracker(std::string const&);
progress_tracker& get_active_progress_tracker();

}  // namespace utl
