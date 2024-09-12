#pragma once

#include <utility>

#include "utl/clear_t.h"
#include "utl/pipes/is_range.h"
#include "utl/pipes/make_range.h"

namespace utl {

template <typename Range, typename Transform>
struct transform_range : public clear_t<Range> {
  using parent_t = clear_t<Range>;
  using result_t = clear_t<decltype(std::declval<Transform>().fn_(
      std::declval<typename parent_t::result_t>()))>;

  template <typename T>
  transform_range(T&& r, Transform&& transform)
      : parent_t(std::forward<T>(r)),
        transform_(std::forward<Transform>(transform)) {}

  template <typename It>
  auto read(It& it) const {
    return transform_.fn_(parent_t::read(it));
  }

  Transform transform_;
};

template <typename Fn>
struct transform_t {
  transform_t(Fn&& f) : fn_(std::forward<Fn>(f)) {}

  template <typename T>
  friend auto operator|(T&& r, transform_t&& f) {
    return transform_range<decltype(make_range(r)), transform_t>(
        std::forward<T>(r), std::move(f));
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
