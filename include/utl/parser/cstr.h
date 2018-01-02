#pragma once

#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>
#include <string>

namespace utl {

struct size {
  explicit size(std::size_t s) : size_(s) {}
  std::size_t size_;
};

struct cstr {
  cstr() : str(nullptr), len(0) {}
  cstr(char const* s) : str(s), len(std::strlen(str)) {}
  cstr(char const* s, std::size_t l) : str(s), len(l) {}
  cstr(unsigned char const* s, std::size_t l)
      : str(reinterpret_cast<char const*>(s)), len(l) {}
  cstr(std::nullptr_t, std::size_t) : str(nullptr), len(0) {}
  cstr& operator++() {
    ++str;
    --len;
    return *this;
  }
  cstr operator+(std::size_t inc) {
    cstr tmp;
    tmp.str = str + inc;
    tmp.len = len - inc;
    return tmp;
  }
  cstr& operator+=(std::size_t inc) {
    str = str + inc;
    len = len - inc;
    return *this;
  }
  bool operator==(cstr const& s) const {
    if (len != s.len) {
      return false;
    } else {
      return strncmp(str, s.str, len) == 0;
    }
  }
  bool operator<(cstr const& s) const {
    return std::lexicographical_compare(str, str + len, s.str, s.str + s.len);
  }
  char operator[](std::size_t i) const { return str[i]; }
  operator bool() { return len != 0 && str != nullptr; }
  char const* begin() const { return str; }
  char const* end() const { return str + len; }
  friend char const* begin(cstr const& s) { return s.begin(); }
  friend char const* end(cstr const& s) { return s.end(); }
  void assign(char const* s, std::size_t l) {
    str = s;
    len = l;
  }
  cstr substr(std::size_t position, size s) const {
    return {str + position, s.size_};
  }
  cstr substr(std::size_t begin, std::size_t end) const {
    return {str + begin, end - begin};
  }
  cstr substr(std::size_t begin) const { return {str + begin, len - begin}; }
  bool starts_with(cstr prefix) const {
    if (len < prefix.len) {
      return false;
    }
    return substr(0, size(prefix.len)) == prefix;
  }
  cstr skip_whitespace_front() {
    auto copy = (*this);
    while (copy.len != 0 && std::isspace(copy[0])) {
      ++copy;
    }
    return copy;
  }
  cstr skip_whitespace_back() {
    auto copy = (*this);
    while (copy.len != 0 && std::isspace(copy.str[copy.len - 1])) {
      --copy.len;
    }
    return copy;
  }
  cstr trim() { return skip_whitespace_front().skip_whitespace_back(); }
  bool empty() const { return len == 0; }
  std::size_t length() const { return len; }
  char const* c_str() const { return str; }
  std::size_t substr_offset(cstr needle) {
    for (std::size_t i = 0; i < len; ++i) {
      if (substr(i).starts_with(needle)) {
        return i;
      }
    }
    return std::numeric_limits<std::size_t>::max();
  }
  std::string to_str() const { return std::string(str, len); }

  char const* str;
  std::size_t len;
};

inline cstr get_until(cstr s, char delimiter) {
  if (s.len == 0) {
    return s;
  }
  auto end = static_cast<char const*>(std::memchr(s.str, delimiter, s.len));
  if (end == nullptr) {
    return s;
  }
  return {s.str, static_cast<std::size_t>(end - s.str)};
}

inline cstr strip_cr(cstr s) {
  return (s && s[s.len - 1] == '\r') ? s.substr(0, size(s.len - 1)) : s;
}

inline cstr get_line(cstr s) { return strip_cr(get_until(s, '\n')); }

template <typename Function>
void for_each_token(cstr s, char separator, Function f) {
  while (s.len > 0) {
    cstr token = get_until(s, separator);
    f(token);
    s += token.len;
    // skip separator
    if (s.len != 0) {
      ++s;
    }
  }
}

template <typename Function>
void for_each_line(cstr s, Function f) {
  for_each_token(s, '\n', [&f](cstr token) { f(strip_cr(token)); });
}

template <typename Function>
void for_each_line_numbered(cstr s, Function f) {
  int line_number = 0;
  for_each_line(s, [&line_number, &f](cstr line) { f(line, ++line_number); });
}

template <typename Predicate>
cstr skip_lines(cstr file_content, Predicate skip) {
  while (file_content.len > 0) {
    cstr line = get_line(file_content);
    if (!skip(line)) {
      break;
    }
    file_content += line.len;
    if (file_content.len != 0) {
      ++file_content;
    }
  }
  return file_content;
}

}  // namespace utl
