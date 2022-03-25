#pragma once

#include "utl/pipes/make_range.h"

#include "utl/clear_t.h"

namespace utl {

template <typename FromIntType, typename ToIntType>
struct iota_range {
  using result_t = FromIntType;

  struct end_it {};

  struct it {
    explicit it(FromIntType v) : val_{std::move(v)} {}
    FromIntType val_;
  };

  iota_range(FromIntType from, ToIntType to)
      : from_(std::forward<FromIntType>(from)),
        to_(std::forward<ToIntType>(to)) {}

  auto begin() { return it{from_}; }
  end_it end() { return end_it{}; }

  template <typename It>
  bool valid(It& it) {
    return it.val_ < to_;
  }

  template <typename It>
  auto&& read(It& it) const {
    return it.val_;
  }

  template <typename It>
  void next(It& it) {
    ++it.val_;
  }

  FromIntType from_;
  ToIntType to_;
};

template <typename FromIntType, typename ToIntType>
iota_range(FromIntType, ToIntType)
    -> iota_range<clear_t<FromIntType>, clear_t<ToIntType>>;

template <typename FromIntType, typename ToIntType>
auto iota(FromIntType&& from, ToIntType&& to) {
  return iota_range{std::forward<FromIntType>(from),
                    std::forward<ToIntType>(to)};
}

template <typename FromIntType, typename ToIntType>
struct is_range<iota_range<FromIntType, ToIntType>> : std::true_type {};

}  // namespace utl
