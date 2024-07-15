#include <coroutine>
#include <exception>

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

  ~generator() { h_.destroy(); }

  explicit operator bool() {
    fill();
    return !h_.done();
  }

  T operator()() {
    fill();
    full_ = false;
    return std::move(h_.promise().value_);
  }

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