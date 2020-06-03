#include "utl/progress_tracker.h"

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <algorithm>

#include "utl/get_or_create.h"
#include "utl/verify.h"

namespace utl {

inline void update_status(progress_tracker& t, bool const active,
                          std::string const& msg) {
  if (t.active_ != active || t.msg_ != msg) {
    t.active_ = active;
    t.msg_ = msg;
    t.callback_(t);
  }
}

void progress_tracker::set_msg(std::string const& msg) {
  update_status(*this, active_, msg);
}

void progress_tracker::activate(std::string const& msg) {
  update_status(*this, true, msg);
}

void progress_tracker::deactivate(std::string const& msg) {
  update_status(*this, false, msg);
}

void progress_tracker::set_bounds(float const out_low, float const out_high) {
  set_bounds(out_low, out_high, 1.F, 100ULL);
}

void progress_tracker::set_bounds(float const out_low, float const out_high,
                                  size_t const in_high) {
  set_bounds(out_low, out_high, 1.F, in_high);
}

void progress_tracker::set_bounds(float const out_low, float const out_high,
                                  float const out_mod, size_t const in_high) {
  if (out_low_ != out_low || out_high_ != out_high || out_mod_ != out_mod ||
      in_high_ != in_high) {
    verify(out_low < out_high,
           "progress_tracker::set_bounds out_low must be lower than out_high");
    verify(out_mod_ > 0.F,
           "progress_tracker::set_bounds out_mod must be greater than zero");
    verify(in_high > 0ULL,
           "progress_tracker::set_bounds in_high must be greater than zero");

    in_ = 0ULL;
    out_ = 0.F;

    out_low_ = out_low;
    out_high_ = out_high;
    out_mod_ = out_mod;
    in_high_ = in_high;

    callback_(*this);
  }
}

void progress_tracker::set_in_bounds(size_t const in_high) {
  if (in_high_ != in_high) {
    in_ = 0ULL;
    out_ = 0.F;

    in_high_ = in_high;

    callback_(*this);
  }
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

progress_tracker& global_progress_trackers::get_tracker(
    std::string const& name) {
  std::lock_guard<std::mutex> lock{mutex_};
  auto const it = trackers_.find(name);
  if (it == end(trackers_)) {
    return trackers_.emplace(name, [this](auto&) { print(); }).first->second;
  } else {
    return it->second;
  }
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
  std::lock_guard<std::mutex> lock{mutex_};

  set_vt100_mode();
  move_cursor_up(last_print_height_);
  for (auto const& [name, t] : trackers_) {
    clear_line();
    if (t.active_) {
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
}

global_progress_trackers& get_global_progress_trackers() {
  static global_progress_trackers singleton;
  return singleton;
}

}  // namespace utl
