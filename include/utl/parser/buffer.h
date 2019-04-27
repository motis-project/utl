#pragma once

#include <cstdlib>
#include <cstring>
#include <new>

#include "utl/parser/util.h"

namespace utl {

struct buffer final {
  buffer() : buf_(nullptr), size_(0) {}

  explicit buffer(std::size_t size) : buf_(malloc(size)), size_(size) {
    verify(buf_ != nullptr, "buffer initialization failed");
  }

  explicit buffer(char const* str) : buffer(std::strlen(str)) {
    std::memcpy(buf_, str, size_);
  }

  buffer(char const* str, std::size_t size) : buffer(size) {
    std::memcpy(buf_, str, size_);
  }

  ~buffer() {
    std::free(buf_);
    buf_ = nullptr;
  }

  buffer(buffer const&) = delete;
  buffer& operator=(buffer const&) = delete;

  buffer(buffer&& o) noexcept : buf_(o.buf_), size_(o.size_) {
    o.buf_ = nullptr;
    o.size_ = 0;
  }

  buffer& operator=(buffer&& o) noexcept {
    buf_ = o.buf_;
    size_ = o.size_;
    o.buf_ = nullptr;
    o.size_ = 0;
    return *this;
  }

  inline std::size_t size() const { return size_; }

  inline unsigned char* data() { return static_cast<unsigned char*>(buf_); }
  inline unsigned char const* data() const {
    return static_cast<unsigned char const*>(buf_);
  }

  inline unsigned char* begin() { return data(); }
  inline unsigned char* end() { return data() + size_; }

  inline void resize(std::size_t new_size) {
    if (void* tmp = std::realloc(buf_, new_size)) {
      buf_ = tmp;
      size_ = new_size;
    } else {
      throw std::bad_alloc();
    }
  }

  void* buf_;
  std::size_t size_;
};

}  // namespace utl
