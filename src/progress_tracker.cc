#include "utl/progress_tracker.h"

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <algorithm>
#include <fstream>
#include <iostream>
#include <system_error>

#include "fmt/chrono.h"
#include "fmt/ostream.h"

#include "utl/get_or_create.h"
#include "utl/helpers/algorithm.h"
#include "utl/to_vec.h"
#include "utl/verify.h"

namespace utl {

void add_timing(std::vector<progress_tracker::step_timing>& timings,
                std::string const& status,
                progress_tracker::clock::duration const duration) {
  auto const it =
      utl::find_if(timings, [&](auto&& t) { return t.status_ == status; });
  if (it == end(timings)) {
    timings.emplace_back(progress_tracker::step_timing{status, duration, 1U});
  } else {
    it->duration_ += duration;
    ++it->count_;
  }
}

std::string format_duration(progress_tracker::clock::duration const d) {
  auto const ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
  if (ms < 1000) {
    return fmt::format("{}ms", ms);
  }
  auto const s = ms / 1000;
  if (s < 60) {
    return fmt::format("{}.{:02}s", s, (ms % 1000) / 10);
  }
  if (s < 3600) {
    return fmt::format("{}m {:02}s", s / 60, s % 60);
  }
  return fmt::format("{}h {:02}m {:02}s", s / 3600, (s % 3600) / 60, s % 60);
}

std::string format_time(std::chrono::system_clock::time_point const t) {
  return fmt::format("{:%FT%TZ}",
                     fmt::gmtime(std::chrono::system_clock::to_time_t(t)));
}

progress_tracker::tracker_update::~tracker_update() {
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

progress_tracker::tracker_update progress_tracker::tracker_update::status(
    std::string const& status) {
  verify(tracker_->status_ != progress_tracker::kFinished ||
             status == progress_tracker::kFinished,
         "progress_tracker: {} is terminal, cannot switch to {}",
         progress_tracker::kFinished, status);
  if (tracker_->status_ != status) {
    tracker_->record_step(clock::now());
  }
  compare_and_update(status_changed_, tracker_->status_, status);
  return std::move(*this);
}

progress_tracker::tracker_update progress_tracker::tracker_update::context(
    std::string const& context) {
  compare_and_update(status_changed_, tracker_->context_, context);
  return std::move(*this);
}

progress_tracker::tracker_update
progress_tracker::tracker_update::show_progress(bool const p) {
  compare_and_update(status_changed_, tracker_->show_progress_, p);
  return std::move(*this);
}

progress_tracker::tracker_update
progress_tracker::tracker_update::reset_bounds() {
  compare_and_update(bounds_changed_, tracker_->out_low_, 0.F);
  compare_and_update(bounds_changed_, tracker_->out_high_, 100.F);
  compare_and_update(bounds_changed_, tracker_->out_mod_, 1.F);
  compare_and_update<size_t>(bounds_changed_, tracker_->in_high_, 100ULL);
  return std::move(*this);
}

progress_tracker::tracker_update progress_tracker::tracker_update::out_bounds(
    float const out_low, float const out_high) {
  verify(out_low <= out_high,
         "progress_tracker::set_bounds out_low must be lower than out_high");
  compare_and_update(bounds_changed_, tracker_->out_low_, out_low);
  compare_and_update(bounds_changed_, tracker_->out_high_, out_high);
  return std::move(*this);
}
progress_tracker::tracker_update progress_tracker::tracker_update::out_mod(
    float const out_mod) {
  verify(out_mod > 0.F,
         "progress_tracker::set_bounds out_mod must be greater than zero");
  compare_and_update(bounds_changed_, tracker_->out_mod_, out_mod);
  return std::move(*this);
}

progress_tracker::tracker_update progress_tracker::tracker_update::in_high(
    size_t const in_high) {
  compare_and_update(bounds_changed_, tracker_->in_high_, in_high);
  return std::move(*this);
}

progress_tracker::tracker_update progress_tracker::status(
    std::string const& status) {
  return progress_tracker::tracker_update{this}.status(status);
}

progress_tracker::tracker_update progress_tracker::context(
    std::string const& context) {
  return progress_tracker::tracker_update{this}.context(context);
}

progress_tracker::tracker_update progress_tracker::show_progress(bool const p) {
  return progress_tracker::tracker_update{this}.show_progress(p);
}

progress_tracker::tracker_update progress_tracker::reset_bounds() {
  return progress_tracker::tracker_update{this}.reset_bounds();
}
progress_tracker::tracker_update progress_tracker::out_bounds(
    float const out_low, float const out_high) {
  return progress_tracker::tracker_update{this}.out_bounds(out_low, out_high);
}
progress_tracker::tracker_update progress_tracker::out_mod(
    float const out_mod) {
  return progress_tracker::tracker_update{this}.out_mod(out_mod);
}
progress_tracker::tracker_update progress_tracker::in_high(
    size_t const in_high) {
  return progress_tracker::tracker_update{this}.in_high(in_high);
}

void progress_tracker::update(size_t const new_in) {
  in_ = new_in;
  update_out();
}

void progress_tracker::update_monotonic(size_t const new_in) {
  // see https://stackoverflow.com/a/16190791
  size_t old_in = in_;
  while (old_in < new_in && !in_.compare_exchange_weak(old_in, new_in)) {
    ;
  }

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

progress_tracker::summary progress_tracker::get_summary() const {
  std::lock_guard<std::mutex> lock{mutex_};

  auto const finished = status_ == kFinished;
  auto steps = timings_;
  if (!finished) {
    add_timing(steps, status_, clock::now() - step_start_);
  }

  auto total = clock::duration{0};
  for (auto const& s : steps) {
    total += s.duration_;
  }

  return summary{started_, total, std::move(steps), finished};
}

void progress_tracker::record_step(clock::time_point const now) {
  if (status_ == kFinished) {
    return;
  }
  add_timing(timings_, status_, now - step_start_);
  step_start_ = now;
}

void progress_tracker::restart_timing() {
  std::lock_guard<std::mutex> lock{mutex_};
  if (timings_.empty() && status_ != kFinished) {
    started_ = clock::now();
    step_start_ = started_;
  }
}

inline progress_tracker_ptr get_tracker_unsafe(global_progress_trackers& global,
                                               std::string const& name) {
  auto const it = global.trackers_.find(name);
  if (it == end(global.trackers_)) {
    return global.trackers_
        .emplace(name, std::make_shared<progress_tracker>(
                           [&global](auto&) { global.print(); }))
        .first->second;
  } else {
    return it->second;
  }
};

progress_tracker_ptr global_progress_trackers::get_tracker(
    std::string const& name) {
  std::lock_guard<std::mutex> lock{mutex_};
  return get_tracker_unsafe(*this, name);
};

bool global_progress_trackers::has_trackers() {
  std::lock_guard<std::mutex> lock{mutex_};
  return !trackers_.empty();
}

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
    std::lock_guard<std::mutex> tracker_lock{t->mutex_};
    auto const ctx =
        t->context_.empty() ? "" : fmt::format("{}: ", t->context_);
    if (t->show_progress_) {
      fmt::print(std::cout, "{:>12}: [", name);
      constexpr auto const kWidth = 55U;
      for (auto i = 0U; i < kWidth; ++i) {
        auto const scaled = static_cast<int>(i * 100.0 / kWidth);
        std::cout << (scaled <= t->out_ ? BAR : " ");
      }
      fmt::print(std::cout, " ] {:>3}%", static_cast<int>(t->out_));
      if (!t->status_.empty()) {
        fmt::print(std::cout, " | {}{}", ctx, t->status_);
      }
      std::cout << "\n";
    } else {
      fmt::print(std::cout, "{:>12}: {}{}\n", name, ctx, t->status_);
    }
  }
  std::cout << std::flush;
  last_print_height_ = trackers_.size();
}

void global_progress_trackers::print_summary(
    std::ostream& out, std::chrono::system_clock::time_point const started,
    std::chrono::steady_clock::duration const total_wall) {
  constexpr auto const kWidth = 72U;

  std::lock_guard<std::mutex> lock{mutex_};

  // Get step summaries sorted by start time.
  auto summaries = utl::to_vec(trackers_, [](auto const& e) {
    return std::pair{e.first, e.second->get_summary()};
  });
  utl::sort(summaries, [](auto const& a, auto const& b) {
    return a.second.started_ < b.second.started_;
  });

  fmt::print(out, "{:=^{}}\n", fmt::format(" {} ", format_time(started)),
             kWidth);
  for (auto const& [name, s] : summaries) {
    fmt::print(out, "{:<{}.{}}{:>12}\n",
               s.finished_ ? name : name + " (unfinished)", kWidth - 12,
               kWidth - 13, format_duration(s.total_));

    if (s.steps_.size() < 2U) {
      continue;  // a single step carries no information beyond the total
    }

    for (auto const& step : s.steps_) {
      auto const name =
          step.status_.empty() ? "(no status)" : step.status_.c_str();
      auto const label = step.count_ == 1
                             ? std::string{name}
                             : fmt::format("{} (x{})", name, step.count_);
      fmt::print(out, "  {:.<{}.{}}{:>12}\n", label + " ", kWidth - 14,
                 kWidth - 15, format_duration(step.duration_));
    }
  }

  fmt::print(out, "{:-^{}}\n", "", kWidth);
  fmt::print(out, "{:<{}}{:>12}\n", "total", kWidth - 12,
             format_duration(total_wall));
  out << std::flush;
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

progress_tracker_ptr activate_progress_tracker(progress_tracker_ptr tracker) {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  global.active_tracker_ = tracker;
  tracker->restart_timing();
  return tracker;
}

progress_tracker_ptr activate_progress_tracker(std::string const& name) {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  auto tracker = get_tracker_unsafe(global, name);
  global.active_tracker_ = tracker;
  tracker->restart_timing();
  return tracker;
}

progress_tracker_ptr get_active_progress_tracker() {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  verify(global.active_tracker_ != nullptr,
         "get_active_progress_tracker: there is no active progress_tracker");
  return global.active_tracker_;
}

progress_tracker_ptr get_active_progress_tracker_or_activate(
    std::string const& name) {
  auto& global = get_global_progress_trackers();
  std::lock_guard<std::mutex> lock{global.mutex_};
  if (global.active_tracker_ != nullptr) {
    return global.active_tracker_;
  } else {
    auto tracker = get_tracker_unsafe(global, name);
    global.active_tracker_ = tracker;
    return tracker;
  }
}

global_progress_bars::global_progress_bars(
    bool const silent, std::optional<std::filesystem::path> summary_path)
    : summary_path_{std::move(summary_path)} {
  auto& gp = utl::get_global_progress_trackers();
  old_silent_ = gp.silent_;
  gp.silent_ = silent;
}

global_progress_bars::~global_progress_bars() {
  auto& gp = utl::get_global_progress_trackers();

  if (summary_path_.has_value() && gp.has_trackers()) {
    try {
      auto ec = std::error_code{};
      if (summary_path_->has_parent_path()) {
        std::filesystem::create_directories(summary_path_->parent_path(), ec);
      }

      auto const prev_file_size =
          std::filesystem::file_size(*summary_path_, ec);
      auto f = std::ofstream{*summary_path_, std::ios_base::app};
      if (f.good()) {
        if (!ec && prev_file_size > 0U) {
          f << "\n";  // separate this run from the previous ones
        }

        gp.print_summary(f, start_time_,
                         std::chrono::steady_clock::now() - start_);
      }
    } catch (...) {
    }
  }

  gp.silent_ = old_silent_;
  gp.clear();
}

}  // namespace utl
