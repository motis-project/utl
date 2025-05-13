#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace utl {

struct progress_tracker {
  struct tracker_update {
    explicit tracker_update(progress_tracker* tracker)
        : tracker_{tracker}, lock_{tracker_->mutex_} {}
    ~tracker_update();

    tracker_update(tracker_update const&) = delete;
    tracker_update& operator=(tracker_update const&) = delete;
    tracker_update(tracker_update&&) = default;
    tracker_update& operator=(tracker_update&&) = default;

    tracker_update status(std::string const&);
    tracker_update context(std::string const&);
    tracker_update show_progress(bool);

    tracker_update reset_bounds();
    tracker_update out_bounds(float out_low, float out_high);
    tracker_update out_mod(float out_mod);
    tracker_update in_high(size_t in_high);

    progress_tracker* tracker_;
    std::unique_lock<std::mutex> lock_;
    bool status_changed_{false};
    bool bounds_changed_{false};
  };

  explicit progress_tracker(
      std::function<void(progress_tracker const&)> callback)
      : callback_{std::move(callback)} {}

  tracker_update status(std::string const&);
  tracker_update context(std::string const&);
  tracker_update show_progress(bool);

  tracker_update reset_bounds();
  tracker_update out_bounds(float out_low, float out_high);
  tracker_update out_mod(float out_mod);
  tracker_update in_high(size_t in_high);

  auto update_fn() {
    return [this](size_t new_in) { update_monotonic(new_in); };
  }
  auto increment_fn() {
    return [this]() { increment(); };
  }
  void update(size_t new_in);
  void increment(size_t inc = 1);
  void update_monotonic(size_t new_in);

  void update_out();

  std::function<void(progress_tracker const&)> callback_;

  mutable std::mutex mutex_;
  std::string status_;
  std::string context_;
  bool show_progress_{true};
  float out_low_{0.F};
  float out_high_{100.F};
  float out_mod_{1.F};
  size_t in_high_{100ULL};

  std::atomic_size_t in_{0ULL};
  std::atomic<float> out_{0.F};
};

using progress_tracker_ptr = std::shared_ptr<progress_tracker>;

struct global_progress_trackers {
  global_progress_trackers() = default;
  global_progress_trackers(global_progress_trackers const&) = delete;
  global_progress_trackers& operator=(global_progress_trackers const&) = delete;
  global_progress_trackers(global_progress_trackers&&) = delete;
  global_progress_trackers& operator=(global_progress_trackers&&) = delete;

  progress_tracker_ptr get_tracker(std::string const& name);

  void print();
  void clear();

  std::mutex mutex_;
  bool silent_{true};
  unsigned last_print_height_{0U};
  std::map<std::string, progress_tracker_ptr> trackers_;
  progress_tracker_ptr active_tracker_;
};

global_progress_trackers& get_global_progress_trackers();

progress_tracker_ptr activate_progress_tracker(progress_tracker_ptr);
progress_tracker_ptr activate_progress_tracker(std::string const&);
progress_tracker_ptr get_active_progress_tracker();
progress_tracker_ptr get_active_progress_tracker_or_activate(
    std::string const&);

struct global_progress_bars {
  explicit global_progress_bars(bool silent = false);
  ~global_progress_bars();

  global_progress_bars(global_progress_bars const&) = default;
  global_progress_bars(global_progress_bars&&) = default;
  global_progress_bars& operator=(global_progress_bars const&) = default;
  global_progress_bars& operator=(global_progress_bars&&) = default;

  bool old_silent_;
};

}  // namespace utl
