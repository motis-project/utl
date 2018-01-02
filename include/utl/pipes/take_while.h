#pragma once

#include <utility>

namespace utl {

template <typename Range, typename TakeWhile>
struct take_while_range : public Range {
  take_while_range(Range&& r, TakeWhile&& take_while)
      : Range(std::forward<Range>(r)),
        take_while_(std::forward<TakeWhile>(take_while)) {}

  template <typename It>
  bool valid(It& it) {
    return take_while_.fn_(this->read(it));
  }

  TakeWhile take_while_;
};

template <typename Fn>
struct take_while_t {
  take_while_t(Fn&& f) : fn_(std::forward<Fn>(f)) {}

  template <typename Range>
  friend auto operator|(Range&& r, take_while_t&& f) {
    return take_while_range<Range, take_while_t>(std::forward<Range>(r),
                                                 std::move(f));
  }

  Fn fn_;
};

template <typename TakeWhile>
take_while_t<TakeWhile> take_while(TakeWhile&& f) {
  return take_while_t<TakeWhile>(std::forward<TakeWhile>(f));
}

}  // namespace utl