#pragma once

#include <utility>

namespace utl {

template <typename Range, typename RemoveIf>
struct remove_if_range : public Range {
  remove_if_range(Range&& r, RemoveIf&& remove_if)
      : Range(std::forward<Range>(r)),
        remove_if_(std::forward<RemoveIf>(remove_if)) {}

  template <typename It>
  void find(It& it) {
    while (this->valid(it) && remove_if_.fn_(this->read(it))) {
      Range::next(it);
    }
  }

  template <typename It>
  void next(It& it) {
    Range::next(it);
    find(it);
  }

  auto begin() {
    auto it = Range::begin();
    find(it);
    return it;
  }

  RemoveIf remove_if_;
};

template <typename Fn>
struct remove_if_t {
  remove_if_t(Fn&& f) : fn_(std::forward<Fn>(f)) {}

  template <typename Range>
  friend auto operator|(Range&& r, remove_if_t&& f) {
    return remove_if_range<Range, remove_if_t>(std::forward<Range>(r),
                                               std::move(f));
  }

  Fn fn_;
};

template <typename RemoveIf>
remove_if_t<RemoveIf> remove_if(RemoveIf&& f) {
  return remove_if_t<RemoveIf>(std::forward<RemoveIf>(f));
}

}  // namespace utl