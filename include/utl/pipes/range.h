#pragma once

#include <utility>

#include "utl/clear_t.h"

namespace utl {

template <typename BeginIt, typename EndIt>
struct range {
  using result_t = clear_t<decltype(*std::declval<BeginIt>())>;

  using it_t = BeginIt;
  using begin_t = BeginIt;
  using end_t = EndIt;

  range(BeginIt begin, EndIt end)
      : begin_(std::forward<BeginIt>(begin)), end_(std::forward<EndIt>(end)) {}

  BeginIt begin() { return begin_; }
  EndIt end() { return end_; }

  template <typename It>
  auto&& read(It& it) {
    return *it;
  }

  template <typename It>
  void next(It& it) {
    ++it;
  }

  template <typename It>
  bool valid(It& it) {
    return it != end_;
  }

  BeginIt begin_;
  EndIt end_;
};

#if __cplusplus >= 201703L
template <typename BeginIt, typename EndIt>
range(BeginIt, EndIt)->range<BeginIt, EndIt>;
#endif

}  // namespace utl