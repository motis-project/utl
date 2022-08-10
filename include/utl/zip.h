#pragma once

#include <array>
#include <functional>
#include <stdexcept>

namespace utl {
namespace detail {

template <typename Tuple, typename Func, std::size_t... I>
auto map_tup(std::index_sequence<I...>, Tuple& tuple, Func&& func) {
  return std::make_tuple(func(std::get<I>(tuple))...);
}

template <typename Tuple, typename Func>
auto map_tup(Tuple& tuple, Func&& func) {
  using Sequence = std::make_index_sequence<std::tuple_size<Tuple>::value>;
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
  using Sequence = std::make_index_sequence<std::tuple_size<Tuple>::value>;
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
    return std::forward_as_tuple(*std::get<I>(its_)...);
  }

  References operator*() {
    using Sequence = std::make_index_sequence<sizeof...(Iterators)>;
    return deref_helper(Sequence());
  }

  SelfType& operator++() {
    each_tup(its_, [](auto&& it) { ++it; });
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

template <typename... Containers>
struct zip_range {
  using Iterator = zip_iterator<std::tuple<std::conditional_t<
      std::is_const<std::remove_reference_t<Containers>>::value,
      typename std::remove_reference_t<Containers>::const_iterator,
      typename std::remove_reference_t<Containers>::iterator>...>>;

  using ConstIterator = zip_iterator<std::tuple<
      typename std::remove_reference_t<Containers>::const_iterator...>>;

  explicit zip_range(std::tuple<Containers...> tup) : tup_(std::move(tup)) {}

  ConstIterator begin() const {
    return ConstIterator{map_tup(tup_, [](auto&& c) { return std::begin(c); })};
  }

  auto begin() -> Iterator {
    return Iterator{map_tup(tup_, [](auto&& c) { return std::begin(c); })};
  }

  ConstIterator end() const {
    return ConstIterator{map_tup(tup_, [](auto&& c) { return std::end(c); })};
  }

  auto end() -> Iterator {
    return Iterator{map_tup(tup_, [](auto&& c) { return std::end(c); })};
  }

  std::tuple<Containers...> tup_;
};

template <typename... Containers>
inline void check_dimensions(Containers&&... containers) {
  static_assert(sizeof...(Containers) > 0, "cannot zip nothing ;)");
  std::array<size_t, sizeof...(Containers)> sizes{{containers.size()...}};
  for (auto const& size : sizes) {
    if (size != sizes[0]) {
      throw std::runtime_error("utl::zip container size mismatch");
    }
  }
}

template <typename... Ts>
auto tuple_forward(Ts&&... t) {
  return std::tuple<Ts&&...>{std::forward<Ts>(t)...};
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
  using value_t =
      std::add_rvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>;
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
