#pragma once

#include <coroutine>
#include <exception>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

namespace utl {

template <typename T>
struct generator {
  struct promise_type {
    generator get_return_object() { return handle_type::from_promise(*this); }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void unhandled_exception() { exception_ = std::current_exception(); }

    template <std::convertible_to<T> From>
    std::suspend_always yield_value(From&& from) {
      value_ = std::forward<From>(from);
      return {};
    }

    void return_void() {}

    T value_;
    std::exception_ptr exception_;
  };

  using handle_type = std::coroutine_handle<promise_type>;

  generator(handle_type h) : h_(h) {}

  generator(generator const&) = delete;
  generator& operator=(generator const&) = delete;

  generator(generator&& o) noexcept
      : h_(std::exchange(o.h_, {})), full_(std::exchange(o.full_, false)) {}

  generator& operator=(generator&& o) noexcept {
    if (this != &o) {
      if (h_) {
        h_.destroy();
      }
      h_ = std::exchange(o.h_, {});
      full_ = std::exchange(o.full_, false);
    }
    return *this;
  }

  ~generator() {
    if (h_) {
      h_.destroy();
    }
  }

  explicit operator bool() {
    if (!h_ || h_.done()) {
      return false;
    }
    fill();
    return !h_.done();
  }

  T operator()() {
    fill();
    full_ = false;
    return std::move(h_.promise().value_);
  }

  std::vector<T> to_vec() {
    auto v = std::vector<T>{};
    while ((*this)) {
      v.emplace_back((*this)());
    }
    return v;
  }

  struct sentinel {};

  struct iterator {
    using iterator_concept = std::input_iterator_tag;
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    iterator() = default;
    explicit iterator(generator* g) : gen_{g} {
      if (gen_ != nullptr && static_cast<bool>(*gen_)) {
        current_ = (*gen_)();
      }
    }

    // `std::indirectly_readable` requires `*it` to be valid on a const
    // iterator and to yield the same type as `iter_reference_t<it>` (= `T&`).
    T& operator*() const { return const_cast<T&>(*current_); }

    iterator& operator++() {
      if (gen_ != nullptr && static_cast<bool>(*gen_)) {
        current_ = (*gen_)();
      } else {
        current_.reset();
      }
      return *this;
    }

    void operator++(int) { ++(*this); }

    friend bool operator==(iterator const& it, sentinel) {
      return !it.current_.has_value();
    }
    friend bool operator==(sentinel s, iterator const& it) { return it == s; }

    generator* gen_{nullptr};
    std::optional<T> current_{};
  };

  iterator begin() { return iterator{this}; }
  sentinel end() { return {}; }

private:
  void fill() {
    if (!full_) {
      h_();
      if (h_.promise().exception_) {
        std::rethrow_exception(h_.promise().exception_);
      }
      full_ = true;
    }
  }

  handle_type h_;
  bool full_{false};
};

}  // namespace utl
