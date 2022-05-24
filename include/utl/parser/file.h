#pragma once

#include <string>

#ifdef _MSC_VER
#include <cstdio>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "utl/parser/buffer.h"
#include "utl/verify.h"

namespace utl {

inline HANDLE open_file(char const* path, char const* mode) {
  bool read = std::strcmp(mode, "r") == 0;
  bool write = std::strcmp(mode, "w+") == 0;

  verify(read || write, "invalid open file mode [%s]", mode);

  DWORD access = read ? GENERIC_READ : GENERIC_WRITE;
  DWORD create_mode = read ? OPEN_EXISTING : CREATE_ALWAYS;

  return CreateFileA(path, access, 0, nullptr, create_mode,
                     FILE_ATTRIBUTE_NORMAL, nullptr);
}

template <typename Fn>
inline void chunk(unsigned const chunk_size, size_t const total, Fn fn) {
  size_t offset = 0;
  size_t remaining = total;
  while (remaining > 0) {
    auto const curr_chunk_size = static_cast<unsigned>(
        std::min(remaining, static_cast<size_t>(chunk_size)));
    fn(offset, curr_chunk_size);
    offset += curr_chunk_size;
    remaining -= curr_chunk_size;
  }
}

struct file {
  file(char const* path, char const* mode)
      : f_{open_file(path, mode)}, filename_{path} {
    verify(f_ != INVALID_HANDLE_VALUE, "unable to open file {} (mode={})", path,
           mode);
  }

  ~file() {
    if (f_ != nullptr) {
      CloseHandle(f_);
    }
  }

  size_t size() {
    LARGE_INTEGER filesize;
    check(GetFileSizeEx(f_, &filesize), "get file size error");
    return filesize.QuadPart;
  }

  buffer content() {
    constexpr auto block_size = 8192u;
    size_t const file_size = size();

    auto b = buffer(file_size);

    chunk(block_size, size(), [&](size_t const from, unsigned block_size) {
      OVERLAPPED overlapped = {0};
      overlapped.Offset = static_cast<DWORD>(from);
      overlapped.OffsetHigh = from >> 32u;
      check(ReadFile(f_, b.data() + from, static_cast<DWORD>(block_size),
                     nullptr, &overlapped),
            "file read error");
    });

    return b;
  }

  std::string content_str() {
    constexpr auto block_size = 8192u;
    size_t const file_size = size();

    auto b = std::string{};
    b.resize(file_size);

    chunk(block_size, size(), [&](size_t const from, unsigned block_size) {
      OVERLAPPED overlapped = {0};
      overlapped.Offset = static_cast<DWORD>(from);
      overlapped.OffsetHigh = from >> 32u;
      check(ReadFile(f_, b.data() + from, static_cast<DWORD>(block_size),
                     nullptr, &overlapped),
            "file read error");
    });

    return b;
  }

  void write(void const* buf, size_t size) {
    constexpr auto block_size = 8192u;
    chunk(block_size, size, [&](size_t const from, unsigned block_size) {
      OVERLAPPED overlapped = {0};
      overlapped.Offset = static_cast<DWORD>(from);
      overlapped.OffsetHigh = from >> 32u;
      check(WriteFile(f_, static_cast<unsigned char const*>(buf) + from,
                      block_size, nullptr, &overlapped),
            "file write error");
    });
  }

  void check(BOOL const ok, char const* msg) {
    if (ok == FALSE) {
      auto const err_code = GetLastError();
      char err_msg[2048];
      if (FormatMessageA(
              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK,
              nullptr, err_code, 0, err_msg, sizeof(err_msg), nullptr) != 0) {
        throw fail("{}: {}: {} ({})", msg, filename_, err_msg, err_code);
      } else {
        throw fail("{}: {}: {}", msg, filename_, err_code);
      }
    }
  }

  HANDLE f_;
  std::string filename_;
};
#else
#include <cstdio>

#include "utl/parser/buffer.h"
#include "utl/verify.h"

namespace utl {

struct file {
  file(char const* path, char const* mode)
      : f_{std::fopen(path, mode)}, filename_{path} {
    verify(f_ != nullptr, "unable to open file {} (mode={})", path, mode);
  }

  ~file() {
    if (f_ != nullptr) {
      fclose(f_);
    }
    f_ = nullptr;
  }

  size_t size() {
    auto err = std::fseek(f_, 0, SEEK_END);
    verify(!err, "fseek to SEEK_END error: {}", filename_);
    auto size = std::ftell(f_);
    std::rewind(f_);
    return static_cast<size_t>(size);
  }

  std::string content_str() {
    auto file_size = size();
    auto s = std::string{};
    s.resize(file_size);
    auto bytes_read = std::fread(s.data(), 1, file_size, f_);
    verify(bytes_read == file_size, "file read error: {}", filename_);
    return s;
  }

  buffer content() {
    auto file_size = size();
    auto b = buffer(file_size);
    auto bytes_read = std::fread(b.data(), 1, file_size, f_);
    verify(bytes_read == file_size, "file read error: {}", filename_);
    return b;
  }

  void write(void const* buf, size_t size) {
    auto bytes_written = std::fwrite(buf, 1, size, f_);
    verify(bytes_written == size, "file write error: {}", filename_);
  }

  operator FILE*() { return f_; }

  FILE* f_;
  std::string filename_;
};
#endif

}  // namespace utl
