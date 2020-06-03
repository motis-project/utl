#pragma once

#include <atomic>
#include <functional>
#include <string>

namespace utl {

struct progress_tracker {
  explicit progress_tracker(
      std::function<void(progress_tracker const&)> callback)
      : callback_{std::move(callback)} {}

  void set_msg(std::string const&);
  void activate(std::string const& = "");
  void deactivate(std::string const& = "");

  void set_bounds(float out_low, float out_high);
  void set_bounds(float out_low, float out_high, size_t in_high);
  void set_bounds(float out_low, float out_high, float out_mod, size_t in_high);

  void set_in_bounds(size_t in_high);

  void update(size_t new_in);
  void increment(size_t inc = 1);
  void update_monotonic(size_t new_in);

  void update_out();

  std::function<void(progress_tracker const&)> callback_;

  std::string msg_;
  bool active_{true};
  float out_low_{0.F};
  float out_high_{100.F};
  float out_mod_{1.F};
  size_t in_high_{100ULL};

  std::atomic_size_t in_{0ULL};
  std::atomic<float> out_{0.F};
};

}  // namespace utl