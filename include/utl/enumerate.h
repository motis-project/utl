#pragma once

// source: http://reedbeta.com/blog/python-like-enumerate-in-cpp17/

#include <cstddef>
#include <iterator>
#include <tuple>

#include "utl/forward_type.h"

namespace utl {

template <typename It>
struct iterator {
  using difference_type = std::ptrdiff_t;
  using value_type =
      std::tuple<std::size_t, typename std::iterator_traits<It>::value_type>;
  using iterator_category = std::iterator_traits<It>::iterator_category;
  using reference = value_type;
  using pointer = std::add_pointer_t<value_type>;

  difference_type operator-(iterator const& o) const {
    return static_cast<std::ptrdiff_t>(i_) - static_cast<std::ptrdiff_t>(o.i_);
  }

  bool operator!=(iterator const& o) const { return iter_ != o.iter_; }

  void operator++() {
    ++i_;
    ++iter_;
  }

  void operator--()
    requires(std::bidirectional_iterator<It>)
  {
    --i_;
    --iter_;
  }

  iterator operator++(int) const {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  iterator operator--(int) const
    requires(std::bidirectional_iterator<It>)
  {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  iterator& operator+=(difference_type const n)
    requires(std::random_access_iterator<It>)
  {
    i_ += static_cast<std::size_t>(n);
    iter_ += static_cast<std::iterator_traits<It>::difference_type>(n);
    return *this;
  }

  iterator& operator-=(difference_type const n)
    requires(std::random_access_iterator<It>)
  {
    i_ = static_cast<std::size_t>(static_cast<difference_type>(i_) - n);
    iter_ -= static_cast<std::iterator_traits<It>::difference_type>(n);
    return *this;
  }

  auto operator*() const {
    using Type = std::decay_t<decltype(*iter_)>;
    if constexpr (std::is_pointer_v<std::decay_t<Type>>) {
      return std::make_tuple(i_, &(**iter_));
    } else {
      return std::tuple<std::size_t, forward_type_t<decltype(*iter_)>>{i_,
                                                                       *iter_};
    }
  }

  std::size_t i_;
  It iter_;
};

template <typename T, typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T&& iterable) {
  struct iterable_wrapper {
    auto begin() {
      using std::begin;
      return iterator<TIter>{0U, begin(iterable_)};
    }

    auto end() {
      using std::end;
      return iterator<TIter>{0U, end(iterable_)};
    }

    T iterable_;
  };

  return iterable_wrapper{std::forward<T>(iterable)};
}

}  // namespace utl
