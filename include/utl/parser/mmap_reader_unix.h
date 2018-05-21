#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <stdexcept>
#include <utility>

#include "utl/parser/cstr.h"

namespace utl {

struct mmap_reader {
  struct file {
    explicit file(char const* path)
        : fd_{open(path, O_RDONLY | O_CLOEXEC)}, size_{get_size(fd_)} {
      if (fd_ == -1) {
        throw std::runtime_error("cannot open file");
      }
    }

    file(file&& o) noexcept : fd_{o.fd_}, size_{o.size_} {
      o.fd_ = -1;
      o.size_ = 0;
    }

    file& operator=(file&& o) noexcept {
      fd_ = o.fd_;
      size_ = o.size_;
      o.fd_ = -1;
      o.size_ = 0;
      return *this;
    }

    file(file const&) = delete;
    file& operator=(file const&) = delete;

    ~file() {
      if (fd_ != -1) {
        close(fd_);
      }
    }

    static size_t get_size(int fd) {
      if (fd == -1) {
        return 0;
      } else {
        struct stat sb {};
        fstat(fd, &sb);
        return static_cast<size_t>(sb.st_size);
      }
    }

    int fd_;
    size_t size_;
  };

  struct memory_map {
    explicit memory_map(char const* path)
        : f_{path},
          fmap_{static_cast<char*>(
              mmap(nullptr, f_.size_, PROT_READ, MAP_PRIVATE, f_.fd_, 0))} {
      if (fmap_ == MAP_FAILED) {  // NOLINT
        throw std::runtime_error("cannot memory map file");
      }
    }

    memory_map(memory_map&& o) noexcept
        : f_{std::move(o.f_)}, fmap_{std::move(o.fmap_)} {
      o.fmap_ = nullptr;
    }

    memory_map& operator=(memory_map&& o) noexcept {
      f_ = std::move(o.f_);
      fmap_ = o.fmap_;
      o.fmap_ = nullptr;
      return *this;
    }

    memory_map(memory_map const&) = delete;
    memory_map& operator=(memory_map const&) = delete;

    ~memory_map() {
      if (fmap_ != nullptr && fmap_ != MAP_FAILED) {  // NOLINT
        munmap(fmap_, f_.size_);
      }
    }

    size_t size() const { return f_.size_; }
    char* ptr() const { return fmap_; }

    file f_;
    char* fmap_;
  };

  explicit mmap_reader(char const* filename) : m_(filename), it_(m_.ptr()) {}

  cstr read(size_t const num_bytes) {
    auto const start = it_;
    auto const last = m_.ptr() + m_.size();
    auto const rest_size = static_cast<size_t>(last - start);
    auto const bytes_read = std::min(num_bytes, rest_size);
    it_ += bytes_read;
    return {start, bytes_read};
  }

  cstr read_line() {
    auto const start = it_;
    auto const last = m_.ptr() + m_.size();
    if (it_ == last) {
      return {nullptr, 0};
    }
    auto const num = last - it_;
    auto const nl = static_cast<char*>(memchr(it_, '\n', num));
    if (nl == nullptr) {
      auto const tmp = it_;
      it_ = last;
      return {tmp, static_cast<size_t>(num)};
    }
    auto const length = nl != nullptr ? nl - start : last - it_;
    it_ = nl;
    ++it_;
    return {start, static_cast<size_t>(length)};
  }

  float progress() const {
    return (it_ - m_.ptr()) / static_cast<float>(m_.size());
  }

  memory_map m_;
  char* it_;
};

}  // namespace utl
