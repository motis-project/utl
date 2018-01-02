#pragma once

namespace utl {

template <typename Reader>
struct line_range {
  line_range(Reader&& r) : r_(r) {}

  auto begin() { return r_.read_line(); }

  template <typename It>
  auto&& read(It& it) {
    return it;
  }

  template <typename It>
  void next(It& it) {
    it = r_.read_line();
  }

  template <typename It>
  bool valid(It& it) {
    return it;
  }

  Reader r_;
};

}  // namespace utl