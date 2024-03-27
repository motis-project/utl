#pragma once

#include <array>
#include <functional>
#include <stdexcept>

#include "utl/forward_type.h"

namespace utl {
namespace detail {

template <typename Tuple, typename Func, std::size_t... I>
auto map_tup(std::index_sequence<I...>, Tuple& tuple, Func&& func) {
  return std::make_tuple(func(std::get<I>(tuple))...);
}

template <typename Tuple, typename Func>
auto map_tup(Tuple& tuple, Func&& func) {
  using Sequence = std::make_index_sequence<std::tuple_size_v<Tuple>>;
  return map_tup(Sequence(), tuple, func);
}

template <typename Tuple, typename Func, std::size_t... I>
void each_tup(std::index_sequence<I...>, Tuple& tuple, Func&& func) {
  // see: http://stackoverflow.com/a/26902803
  using expander = int[];
  (void)expander{0, ((void)func(std::get<I>(tuple)), 0)...};
}

template <typename Tuple, typename Func>
void each_tup(Tuple& tuple, Func&& func) {
  using Sequence = std::make_index_sequence<std::tuple_size_v<Tuple>>;
  each_tup(Sequence(), tuple, func);
}

template <typename Tuple>
struct zip_iterator;

template <typename... Iterators>
struct zip_iterator<std::tuple<Iterators...>> {
  using SelfType = zip_iterator<std::tuple<Iterators...>>;

  using References =
      std::tuple<typename std::iterator_traits<Iterators>::reference...>;

  using iterator_category = std::input_iterator_tag;
  using value_type = References;
  using reference = value_type;
  using pointer = value_type*;
  using difference_type = std::ptrdiff_t;

  explicit zip_iterator(std::tuple<Iterators...> its) : its_(its) {}

  template <std::size_t... I>
  auto deref_helper(std::index_sequence<I...>) {
    return References{*std::get<I>(its_)...};
  }

  template <std::size_t... I>
  auto deref_helper(std::index_sequence<I...>) const {
    return References{*std::get<I>(its_)...};
  }

  References operator*() {
    using Sequence = std::make_index_sequence<sizeof...(Iterators)>;
    return deref_helper(Sequence());
  }

  References operator*() const {
    using Sequence = std::make_index_sequence<sizeof...(Iterators)>;
    return deref_helper(Sequence());
  }

  SelfType& operator++() {
    each_tup(its_, [](auto&& it) { ++it; });
    return *this;
  }

  SelfType& operator--() {
    each_tup(its_, [](auto&& it) { --it; });
    return *this;
  }

  bool operator==(SelfType const& rhs) const { return its_ == rhs.its_; }
  bool operator!=(SelfType const& rhs) const { return its_ != rhs.its_; }
  bool operator>(SelfType const& rhs) const { return its_ > rhs.its_; }
  bool operator<(SelfType const& rhs) const { return its_ < rhs.its_; }
  bool operator>=(SelfType const& rhs) const { return its_ >= rhs.its_; }
  bool operator<=(SelfType const& rhs) const { return its_ <= rhs.its_; }

  std::tuple<Iterators...> its_;
};

template <class T, class R = void>
struct enable_if_type {
  using type = R;
};

template <typename T, typename = void>
struct has_const_iterator : std::false_type {};

template <typename T>
struct has_const_iterator<
    T, typename enable_if_type<typename T::const_iterator>::type>
    : std::true_type {};

template <typename T>
inline constexpr bool has_const_iterator_v = has_const_iterator<T>::value;

template <typename Container, typename Enable = void>
struct iterator {
  using type = typename Container::iterator;
};

template <typename Container>
struct iterator<
    Container,
    std::enable_if_t<std::is_const_v<std::remove_reference_t<Container>> &&
                     has_const_iterator_v<Container>>> {
  using type = typename Container::const_iterator;
};

template <typename T>
using iterator_t = typename iterator<T>::type;

template <typename Container, typename Enable = void>
struct const_iterator {
  using type = typename Container::iterator;
};

template <typename Container>
struct const_iterator<Container,
                      std::enable_if_t<has_const_iterator_v<Container>>> {
  using type = typename Container::const_iterator;
};

template <typename T>
using const_iterator_t = typename const_iterator<T>::type;

template <typename... Containers>
struct zip_range {
  using iterator = zip_iterator<
      std::tuple<iterator_t<typename std::remove_reference_t<Containers>>...>>;
  using const_iterator = zip_iterator<std::tuple<
      const_iterator_t<typename std::remove_reference_t<Containers>>...>>;

  explicit zip_range(std::tuple<Containers...> tup) : tup_(std::move(tup)) {}

  typename const_iterator::References operator[](std::size_t const i) const {
    return *std::next(begin(),
                      static_cast<typename iterator::difference_type>(i));
  }

  typename iterator::References operator[](std::size_t const i) {
    return *std::next(begin(),
                      static_cast<typename iterator::difference_type>(i));
  }

  std::size_t size() const { return std::get<0>(tup_).size(); }

  const_iterator begin() const {
    return const_iterator{
        map_tup(tup_, [](auto&& c) { return std::begin(c); })};
  }

  auto begin() -> iterator {
    return iterator{map_tup(tup_, [](auto&& c) { return std::begin(c); })};
  }

  const_iterator end() const {
    return const_iterator{map_tup(tup_, [](auto&& c) { return std::end(c); })};
  }

  auto end() -> iterator {
    return iterator{map_tup(tup_, [](auto&& c) { return std::end(c); })};
  }

  friend auto begin(zip_range const& zr) { return zr.begin(); }
  friend auto end(zip_range const& zr) { return zr.end(); }
  friend auto begin(zip_range& zr) { return zr.begin(); }
  friend auto end(zip_range& zr) { return zr.end(); }

  std::tuple<Containers...> tup_;
};

template <typename... Containers>
zip_range(std::tuple<Containers...>) -> zip_range<Containers...>;

template <typename... Containers>
inline void check_dimensions(Containers&&... containers) {
  static_assert(sizeof...(Containers) > 0, "cannot zip nothing ;)");
  std::array<size_t, sizeof...(Containers)> sizes{{containers.size()...}};
  for (auto const& size : sizes) {
    if (size != sizes[0]) {
      throw utl::fail("utl::zip container size mismatch {} vs {}", size,
                      sizes[0]);
    }
  }
}

template <typename... Ts>
auto tuple_forward(Ts&&... t) {
  return std::tuple<forward_type_t<Ts&&>...>{std::forward<Ts&&>(t)...};
}

template <typename T, typename Enable = void>
struct add_const_s;

template <typename T>
struct add_const_s<T, std::enable_if_t<std::is_lvalue_reference_v<T>>> {
  using value_t =
      std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>;
};

template <typename T>
struct add_const_s<T, std::enable_if_t<std::is_rvalue_reference_v<T>>> {
  using value_t = std::add_const_t<std::remove_reference_t<T>>;
};

template <typename T>
struct add_const_s<T, std::enable_if_t<!std::is_reference_v<T>>> {
  using value_t = std::add_const_t<T>;
};

template <typename T>
using add_const = typename add_const_s<T>::value_t;

template <typename... Ts>
auto tuple_const_forward(Ts&&... t) {
  return std::tuple<add_const<Ts&&>...>{std::forward<Ts&&>(t)...};
}

}  // namespace detail

template <typename... Containers>
auto zip_no_size_check(Containers&&... containers) {
  return detail::zip_range{
      detail::tuple_forward(std::forward<Containers&&>(containers)...)};
}

template <typename... Containers>
auto czip_no_size_check(Containers&&... containers) {
  return detail::zip_range{
      detail::tuple_const_forward(std::forward<Containers&&>(containers)...)};
}

template <typename... Containers>
auto zip(Containers&&... containers) {
  detail::check_dimensions(containers...);
  return detail::zip_range{
      detail::tuple_forward(std::forward<Containers&&>(containers)...)};
}

template <typename... Containers>
auto czip(Containers&&... containers) {
  detail::check_dimensions(containers...);
  return detail::zip_range{
      detail::tuple_const_forward(std::forward<Containers&&>(containers)...)};
}

}  // namespace utl
