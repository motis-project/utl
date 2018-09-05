#pragma once

#include <cstring>
#include <string_view>

namespace utl {

struct string {
  using size_t = uint32_t;

  static constexpr struct owning_t {
  } owning;
  static constexpr struct non_owning_t {
  } non_owning;

  string() { std::memset(this, 0, sizeof(*this)); }
  ~string() { reset(); }

  string(char const* s, owning_t) : string() { set_owning(s, std::strlen(s)); }
  string(char const* s, non_owning_t) : string() { set_non_owning(s); }
  string(char const* s) : string(s, non_owning) {}  // NOLINT

  string(string const& s) : string() { copy_from(s); }

  string(string&& s) {
    std::memcpy(this, &s, sizeof(*this));
    std::memset(&s, 0, sizeof(*this));
  }

  string& operator=(string const& s) {
    copy_from(s);
    return *this;
  }

  string& operator=(string&& s) {
    std::memcpy(this, &s, sizeof(*this));
    return *this;
  }

  bool is_short() const { return s_.is_short_; }

  void reset() {
    if (!h_.is_short_ && h_.ptr_ != nullptr && h_.self_allocated_) {
      std::free(const_cast<char*>(h_.ptr_));
    }
    std::memset(this, 0, sizeof(*this));
  }

  void set_owning(std::string_view s) { set_owning(s.data(), s.size()); }

  void set_owning(char const* str) { set_owning(str, std::strlen(str)); }

  void set_owning(char const* str, size_t const size) {
    reset();
    if (str == nullptr || size == 0) {
      return;
    }
    s_.is_short_ = (size <= 15);
    if (s_.is_short_) {
      std::memcpy(s_.s_, str, size);
    } else {
      h_.ptr_ = static_cast<char*>(std::malloc(size));
      if (h_.ptr_ == nullptr) {
        throw std::bad_alloc{};
      }
      h_.size_ = size;
      h_.self_allocated_ = true;
      std::memcpy(const_cast<char*>(h_.ptr_), str, size);
    }
  }

  void set_non_owning(std::string_view v) {
    set_non_owning(v.data(), v.size());
  }

  void set_non_owning(char const* str) {
    set_non_owning(str, std::strlen(str));
  }

  void set_non_owning(char const* str, size_t const size) {
    reset();
    if (str == nullptr || size == 0) {
      return;
    }

    if (size <= 15) {
      return set_owning(str, size);
    }

    h_.is_short_ = false;
    h_.self_allocated_ = false;
    h_.ptr_ = str;
    h_.size_ = size;
  }

  void copy_from(string const& s) {
    reset();
    if (s.is_short()) {
      std::memcpy(this, &s, sizeof(s));
    } else if (s.h_.self_allocated_) {
      set_owning(s.data(), s.size());
    } else {
      set_non_owning(s.data(), s.size());
    }
  }

  std::string_view view() const { return std::string_view{data(), size()}; }

  char const* data() const { return is_short() ? s_.s_ : h_.ptr_; }

  string& operator=(char const* s) {
    set_non_owning(s, std::strlen(s));
    return *this;
  }

  friend bool operator==(string const& a, char const* b) {
    return a.view() == std::string_view{b};
  }

  friend bool operator==(string const& a, string const& b) {
    return a.view() == b.view();
  }

  size_t size() const {
    if (is_short()) {
      auto const pos = static_cast<char const*>(std::memchr(s_.s_, '\0', 15));
      if (pos == nullptr) {
        return 15;
      } else {
        return pos - s_.s_;
      }
    } else {
      return h_.size_;
    }
  }

  union {
    struct {
      bool is_short_;
      bool self_allocated_;
      uint8_t __fill_2__;
      uint8_t __fill_3__;
      uint32_t size_;
      char const* ptr_;
    } h_;
    struct {
      bool is_short_;
      char s_[15];
    } s_;
  };
};

}  // namespace utl