#pragma once

#include <type_traits>
#include <utility>

#include "utl/pipes/is_range.h"

namespace utl {

template <typename Fn>
struct generator_range {
  struct end_it {};

  template <typename Value>
  struct it {
    explicit it(Value&& v) : val_{std::forward<Value>(v)} {}
    Value val_;
  };

  generator_range(Fn&& fn) : fn_(std::forward<Fn>(fn)) {}

  auto begin() { return it<typename std::result_of<Fn()>::type>{fn_()}; }
  end_it end() { return end_it{}; }

  template <typename It>
  auto&& read(It& it) {
    return it.val_;
  }

  template <typename It>
  void next(It& it) {
    it.val_ = fn_();
  }

  template <typename It>
  bool valid(It&) {
    return true;
  }

  Fn fn_;
};

template <typename Fn>
auto generate(Fn&& fn) {
  return generator_range<Fn>{std::forward<Fn>(fn)};
}

template <typename Fn>
struct is_range<generator_range<Fn>> : std::true_type {};

}  // namespace utl