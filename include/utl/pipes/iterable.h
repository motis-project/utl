#pragma once

#include <cstddef>

namespace utl {

struct end_it {};

template <typename Range, typename BaseIt>
struct it {
  using iterator_category = std::input_iterator_tag;
  using value_type = typename Range::result_t;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  bool operator!=(end_it const&) const { return r_.valid(base_it_); }
  value_type operator*() const { return r_.read(base_it_); }
  reference operator->() const { return r_.read(base_it_); }
  void operator++() { r_.next(base_it_); }

  Range& r_;
  BaseIt base_it_;
};

template <typename Range, typename BaseIt>
it(Range, BaseIt)->it<Range, BaseIt>;

template <typename Range>
struct iterable_range : public clear_t<Range> {
  using parent_t = clear_t<Range>;
  iterable_range() = default;
  explicit iterable_range(Range&& r) : parent_t(std::forward<parent_t>(r)) {}
  auto begin() { return it{*this, parent_t::begin()}; }
  auto end() { return end_it{}; }
  friend auto begin(iterable_range const& r) { return r.begin(); }
  friend auto end(iterable_range const& r) { return r.end(); }
};

struct iterable_t {
  template <typename T>
  friend auto operator|(T&& r, iterable_t&&) {
    return iterable_range<decltype(make_range(r))>(
        make_range(std::forward<T>(r)));
  }
};

inline iterable_t iterable() { return iterable_t(); }

}  // namespace utl