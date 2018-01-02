#pragma once

#include <utility>

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

  template <typename Range>
  friend auto operator|(Range&& r, transform_t&& f) {
    return transform_range<Range, transform_t>(std::forward<Range>(r),
                                               std::move(f));
  }

  Fn fn_;
};

template <typename Transform>
transform_t<Transform> transform(Transform&& f) {
  return transform_t<Transform>(std::forward<Transform>(f));
}

}  // namespace utl