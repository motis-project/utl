#pragma once

#include <array>
#include <tuple>

namespace utl {

template <typename It, std::size_t... I>
constexpr auto value_type_helper(std::index_sequence<I...>) -> std::tuple<
    std::add_lvalue_reference_t<decltype(I, *std::declval<It>())>...> {}

template <std::size_t N, typename It>
struct nwise_iterator {
  using indices_t = std::make_index_sequence<N>;

  using iterator_category = std::forward_iterator_tag;

  using difference_type = std::ptrdiff_t;

  using value_type = decltype(value_type_helper<It>(indices_t{}));
  using pointer = value_type const*;
  using reference = value_type const&;

  nwise_iterator(It const begin, It const end) {
    auto curr = begin;
    for (auto& it : its_) {
      it = curr;
      if (curr != end) {
        ++curr;
      }
    }
  }

  template <std::size_t... I>
  auto deref(std::index_sequence<I...>) -> std::tuple<
      std::add_lvalue_reference_t<decltype(I, *std::declval<It>())>...> {
    return {*its_[I]...};
  }

  auto operator*() { return deref(indices_t{}); }

  bool operator==(nwise_iterator const& o) const {
    return its_.back() == o.its_.back();
  }
  bool operator!=(nwise_iterator const& o) const {
    return its_.back() != o.its_.back();
  }

  nwise_iterator& operator++() {
    for (auto& it : its_) {
      ++it;
    }
    return *this;
  }

  std::array<It, N> its_;
};

template <std::size_t N, typename It>
struct nwise_range {
  nwise_range(It begin, It end) : begin_{begin}, end_{end} {}
  nwise_iterator<N, It> begin() const { return {begin_, end_}; }
  nwise_iterator<N, It> end() const { return {end_, end_}; }
  It begin_, end_;
};

template <std::size_t N, typename Container>
auto nwise(Container& c) -> nwise_range<N, decltype(begin(c))> {
  return {c.begin(), c.end()};
}

}  // namespace utl