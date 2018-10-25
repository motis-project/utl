#pragma once

#include "utl/parser/cstr.h"

namespace utl {

struct buf_reader {
  buf_reader(cstr s) : s_{s}, it_{s_.c_str()} {}

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
    }

    auto const length = nl != nullptr ? nl - start : last - it_;
    it_ = nl;
    ++it_;
    return {start, static_cast<size_t>(length)};
  }

  cstr s_;
  char const* it_;
};

}  // namespace utl