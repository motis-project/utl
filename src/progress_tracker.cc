#include "utl/progress_tracker.h"

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <algorithm>

#include "utl/get_or_create.h"
#include "utl/verify.h"

namespace utl {

progress_tracker::txn::~txn() {
  if (lock_.owns_lock()) {  // otherwise: moved from
    if (bounds_changed_) {
      tracker_->in_ = 0ULL;
      tracker_->out_ = tracker_->out_low_;
    }
    lock_.unlock();
    if (status_changed_ || bounds_changed_) {
      tracker_->callback_(*tracker_);
    }
  }
}

template <typename T>
void compare_and_update(bool& changed, T& old_value, T const& new_value) {
  if (old_value != new_value) {
    old_value = new_value;
    changed = true;
  }
}

progress_tracker::txn progress_tracker::txn::msg(std::string const& msg) {
  compare_and_update(status_changed_, tracker_->msg_, msg);
  return std::move(*this);
}
progress_tracker::txn progress_tracker::txn::show_progress(bool const p) {
  compare_and_update(status_changed_, tracker_->show_progress_, p);
  return std::move(*this);
}

progress_tracker::txn progress_tracker::txn::reset_bounds() {
  compare_and_update(bounds_changed_, tracker_->out_low_, 0.F);
  compare_and_update(bounds_changed_, tracker_->out_high_, 100.F);
  compare_and_update(bounds_changed_, tracker_->out_mod_, 1.F);
  compare_and_update<size_t>(bounds_changed_, tracker_->in_high_, 100ULL);
  return std::move(*this);
}

progress_tracker::txn progress_tracker::txn::out_bounds(float const out_low,
                                                        float const out_high) {
  verify(out_low < out_high,
         "progress_tracker::set_bounds out_low must be lower than out_high");
  compare_and_update(bounds_changed_, tracker_->out_low_, out_low);
  compare_and_update(bounds_changed_, tracker_->out_high_, out_high);
  return std::move(*this);
}
progress_tracker::txn progress_tracker::txn::out_mod(float const out_mod) {
  verify(out_mod > 0.F,
         "progress_tracker::set_bounds out_mod must be greater than zero");
  compare_and_update(bounds_changed_, tracker_->out_mod_, out_mod);
  return std::move(*this);
}

progress_tracker::txn progress_tracker::txn::in_high(size_t const in_high) {
  verify(in_high > 0ULL,
         "progress_tracker::set_bounds in_high must be greater than zero");
  compare_and_update(bounds_changed_, tracker_->in_high_, in_high);
  return std::move(*this);
}

progress_tracker::txn progress_tracker::msg(std::string const& msg) {
  return progress_tracker::txn{this}.msg(msg);
}
progress_tracker::txn progress_tracker::show_progress(bool const p) {
  return progress_tracker::txn{this}.show_progress(p);
}

progress_tracker::txn progress_tracker::reset_bounds() {
  return progress_tracker::txn{this}.reset_bounds();
}
progress_tracker::txn progress_tracker::out_bounds(float const out_low,
                                                   float const out_high) {
  return progress_tracker::txn{this}.out_bounds(out_low, out_high);
}
progress_tracker::txn progress_tracker::out_mod(float const out_mod) {
  return progress_tracker::txn{this}.out_mod(out_mod);
}
progress_tracker::txn progress_tracker::in_high(size_t const in_high) {
  return progress_tracker::txn{this}.in_high(in_high);
}

void progress_tracker::update(size_t const new_in) {
  in_ = new_in;
  update_out();
}

void progress_tracker::update_monotonic(size_t const new_in) {
  // see https://stackoverflow.com/a/16190791
  size_t old_in = in_;
  while (old_in < new_in && !in_.compare_exchange_weak(old_in, new_in))
    ;

  update_out();
}

void progress_tracker::increment(size_t const inc) {
  in_ += inc;
  update_out();
}

void progress_tracker::update_out() {
  auto const out_range = (out_high_ - out_low_);
  auto const in_ratio = static_cast<float>(static_cast<double>(in_) /
                                           static_cast<double>(in_high_));

  auto const new_out = std::clamp(
      out_low_ + std::round(out_range * in_ratio / out_mod_) * out_mod_,
      out_low_, out_high_);

  auto old_out = out_.exchange(new_out);
  if (old_out != new_out) {
    callback_(*this);
  }
}

inline progress_tracker& get_tracker_unsafe(global_progress_trackers& global,
                                            std::string const& name) {
  auto const it = global.trackers_.find(name);
  if (it == end(global.trackers_)) {
    return global.trackers_.emplace(name, [&global](auto&) { global.print(); })
        .first->second;
  } else {
    return it->second;
  }
};

progress_tracker& global_progress_trackers::get_tracker(
    std::string const& name) {
  std::lock_guard<std::mutex> lock{mutex_};
  return get_tracker_unsafe(*this, name);
};

#ifdef _MSC_VER

constexpr auto const BAR = "\xDB";

void move(int x, int y) {
  auto hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  if (!hStdout) {
    return;
  }

  CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
  GetConsoleScreenBufferInfo(hStdout, &csbiInfo);

  COORD cursor;
  cursor.X = csbiInfo.dwCursorPosition.X + x;
  cursor.Y = csbiInfo.dwCursorPosition.Y + y;
  SetConsoleCursorPosition(hStdout, cursor);
}

void move_cursor_up(unsigned lines) {
  if (lines != 0) {
    move(0, -lines);
  }
}

void set_vt100_mode() {
  auto hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleMode(hStdout,
                 ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

#else

constexpr auto const BAR = "■";

void set_vt100_mode() {}

void move_cursor_up(unsigned lines) {
  if (lines != 0) {
    std::cout << "\x1b[" << lines << "A";
  }
}

#endif

void clear_line() { std::cout << "\x1b[K"; }

void global_progress_trackers::print() {
  if (silent_) {
    return;
  }
  std::lock_guard<std::mutex> global_lock{mutex_};

  set_vt100_mode();
  move_cursor_up(last_print_height_);
  for (auto const& [name, t] : trackers_) {
    clear_line();
    std::lock_guard<std::mutex> tracker_lock{t.mutex_};
    if (t.show_progress_) {
      fmt::print(std::cout, "{:>12}: [", name);
      constexpr auto const WIDTH = 55U;
      for (auto i = 0U; i < 55U; ++i) {
        auto const scaled = static_cast<int>(i * 100.0 / WIDTH);
        std::cout << (scaled <= t.out_ ? BAR : " ");
      }
      fmt::print(std::cout, "] {:>3.0}%", t.out_);
      if (!t.msg_.empty()) {
        fmt::print(std::cout, " | {}", t.msg_);
      }
      std::cout << "\n";
    } else {
      fmt::print(std::cout, "{:>12}: {}\n", name, t.msg_);
    }
  }
  std::cout << std::flush;
  last_print_height_ = trackers_.size();
}

void global_progress_trackers::clear() {
  std::lock_guard<std::mutex> lock{mutex_};
  trackers_.clear();
  last_print_height_ = 0U;
  active_tracker_ = nullptr;
}

global_progress_trackers& get_global_progress_trackers() {
  static global_progress_trackers singleton;
  return singleton;
}

progress_tracker& activate_progress_tracker(std::string const& name) {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  auto& tracker = get_tracker_unsafe(global, name);
  global.active_tracker_ = &tracker;
  return tracker;
}

progress_tracker& get_active_progress_tracker() {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  verify(global.active_tracker_ != nullptr,
         "get_active_progress_tracker: there is no active progress_tracker");
  return *global.active_tracker_;
}

}  // namespace utl
