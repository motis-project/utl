#pragma once

#include "utl/parser/cstr.h"

namespace utl {

struct noop_progress_consumer {
  void operator()(std::size_t) {}
};

template <typename ProgressConsumer = noop_progress_consumer>
struct buf_reader {
  explicit buf_reader(cstr s) : s_{s}, it_{s_.c_str()} {}
  buf_reader(cstr s, ProgressConsumer consumer)
      : s_{s}, it_{s_.c_str()}, progress_consumer_{std::move(consumer)} {}

  cstr read_line() {
    auto const start = it_;
    auto const last = s_.c_str() + s_.length();
    if (it_ == last) {
      return {nullptr, 0};
    }
    auto const num = static_cast<size_t>(last - it_);
    auto const nl = static_cast<char const*>(memchr(it_, '\n', num));
    if (nl == nullptr) {
      auto const tmp = it_;
      it_ = last;
      return {tmp, static_cast<size_t>(num)};
    } else {
      auto const length = nl - start;
      it_ = nl;
      ++it_;
      update_progress();
      return {start, static_cast<size_t>(length)};
    }
  }

  void update_progress() { progress_consumer_(it_ - s_.c_str()); }

  cstr s_;
  char const* it_;
  ProgressConsumer progress_consumer_;
};

template <typename ProgressConsumer = noop_progress_consumer>
buf_reader<ProgressConsumer> make_buf_reader(
    cstr s, ProgressConsumer&& progress_consumer) {
  return buf_reader<ProgressConsumer>{
      s, std::forward<ProgressConsumer>(progress_consumer)};
}

}  // namespace utl