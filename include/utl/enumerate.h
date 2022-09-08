#pragma once

// source: http://reedbeta.com/blog/python-like-enumerate-in-cpp17/

#include <cstddef>
#include <tuple>

#include "utl/forward_type.h"

namespace utl {

template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T&& iterable) {
  struct iterator {
    bool operator!=(iterator const& o) const { return iter_ != o.iter_; }

    void operator++() {
      ++i_;
      ++iter_;
    }

    auto operator*() const {
      using Type = std::decay_t<decltype(*iter_)>;
      if constexpr (std::is_pointer_v<std::decay_t<Type>>) {
        return std::make_tuple(i_, &(**iter_));
      } else {
        return std::tuple<std::size_t, forward_type_t<decltype(*iter_)>>{
            i_, *iter_};
      }
    }

    std::size_t i_;
    TIter iter_;
  };

  struct iterable_wrapper {
    auto begin() { return iterator{0U, std::begin(iterable_)}; }
    auto end() { return iterator{0U, std::end(iterable_)}; }
    T iterable_;
  };

  return iterable_wrapper{std::forward<T>(iterable)};
}

}  // namespace utl
