#pragma once

#include <utility>

#include "utl/pipes/make_range.h"

namespace utl {

template <typename Range, typename Transform>
struct transform_range : public Range {
  transform_range(Range&& r, Transform&& transform)
      : Range(std::forward<Range>(r)),
        transform_(std::forward<Transform>(transform)) {}

  template <typename It>
  auto read(It& it) {
    return transform_.fn_(Range::read(it));
  }

  Transform transform_;
};

template <typename Fn>
struct transform_t {
  transform_t(Fn&& f) : fn_(std::forward<Fn>(f)) {}

  template <typename T>
  friend auto operator|(T&& r, transform_t&& f) {
    using Range = decltype(make_range(r));
    return transform_range<Range, transform_t>(make_range(std::forward<T>(r)),
                                               std::move(f));
  }

  Fn fn_;
};

template <typename Transform>
transform_t<Transform> transform(Transform&& f) {
  return transform_t<Transform>(std::forward<Transform>(f));
}

template <typename Range, typename Transform>
struct is_range<transform_range<Range, Transform>> : std::true_type {};

}  // namespace utl