#pragma once

#include <utility>

#include "utl/clear_t.h"
#include "utl/pipes/all.h"
#include "utl/pipes/make_range.h"

namespace utl {

template <typename Range>
struct flatten_range : public clear_t<Range> {
  using parent_t = clear_t<Range>;
  using parent_result_t = typename parent_t::result_t;
  using parent_it_t = decltype(std::declval<parent_t>().begin());
  using flattened_it_t = typename parent_result_t::const_iterator;
  using result_t = clear_t<decltype(*std::declval<flattened_it_t>())>;

  explicit flatten_range(Range&& r) : parent_t(std::forward<parent_t>(r)) {}

  template <typename It,
            std::enable_if_t<std::is_reference_v<decltype(*std::declval<It>())>,
                             int> = 0>
  auto&& read(It& it) const {
    return *it;
  }

  template <typename It,
            std::enable_if_t<
                !std::is_reference_v<decltype(*std::declval<It>())>, int> = 0>
  auto read(It& it) const {
    return *it;
  }

  void refresh_currents() {
    current_parent_result_ = parent_t::read(current_parent_it_);
    current_begin_ = current_parent_result_.begin();
    current_end_ = current_parent_result_.end();
  }

  auto begin() {
    current_parent_it_ = parent_t::begin();
    refresh_currents();
    return current_begin_;
  }

  auto end() { return total_end_; }

  template <typename It>
  void move_to_next_parent_it(It& it) {
    parent_t::next(current_parent_it_);

    if (current_parent_it_ != parent_t::end()) {
      refresh_currents();
      it = current_begin_;
    } else {
      it = total_end_;
    }
  }

  template <typename It>
  void next(It& it) {
    if (it != current_end_) {
      ++it;
    }

    if (it == current_end_ && current_parent_it_ != parent_t::end()) {
      move_to_next_parent_it(it);
    }
  }

  template <typename It>
  bool valid(It& it) {
    return it != total_end_;
  }

  parent_it_t current_parent_it_;
  parent_result_t current_parent_result_;

  flattened_it_t current_begin_;
  flattened_it_t current_end_;
  flattened_it_t total_end_ = static_cast<flattened_it_t>(nullptr);
};

struct flatten_t {
  template <typename T>
  friend auto operator|(T&& r, flatten_t&&) {
    return flatten_range<decltype(make_range(r))>(
        make_range(std::forward<T>(r)));
  }
};

inline flatten_t flatten() { return {}; }

template <typename Range>
struct is_range<flatten_range<Range>> : std::true_type {};

}  // namespace utl
