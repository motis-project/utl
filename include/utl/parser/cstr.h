#pragma once

#include <cassert>
#include <cctype>
#include <cstring>

#include <algorithm>
#include <limits>
#include <string>
#include <string_view>

namespace utl {

struct size {
  explicit size(size_t s) : size_(s) {}
  size_t size_;
};

struct field {
  size_t from = 0;
  size_t size = 0;
  static constexpr auto MAX_SIZE = std::numeric_limits<int>::max();
};

struct cstr {
  cstr() : str(nullptr), len(0) {}
  cstr(std::string const& s) : str(s.data()), len(s.length()) {}
  cstr(char const* s) : str(s), len(s ? std::strlen(str) : 0) {}
  cstr(char const* s, size_t l) : str(s), len(l) {}
  cstr(unsigned char const* s, size_t l)
      : str(reinterpret_cast<char const*>(s)), len(l) {}
  cstr(char const* begin, char const* end)
      : str(begin), len(static_cast<size_t>(end - begin)) {
    assert(begin <= end);
  }
  cstr(std::nullptr_t, size_t) : str(nullptr), len(0) {}
  cstr& operator++() {
    ++str;
    --len;
    return *this;
  }
  cstr operator+(size_t inc) {
    cstr tmp;
    tmp.str = str + inc;
    tmp.len = len - inc;
    return tmp;
  }
  cstr& operator+=(size_t inc) {
    str = str + inc;
    len = len - inc;
    return *this;
  }
  bool operator==(cstr const& s) const {
    if (len != s.len) {
      return false;
    } else if (len == 0) {
      return true;
    } else {
      return strncmp(str, s.str, len) == 0;
    }
  }
  bool operator<(cstr const& s) const {
    return std::lexicographical_compare(str, str + len, s.str, s.str + s.len);
  }
  char operator[](size_t i) const { return str[i]; }
  operator bool() const { return len != 0 && str != nullptr; }
  char const* begin() const { return str; }
  char const* end() const { return str + len; }
  friend char const* begin(cstr const& s) { return s.begin(); }
  friend char const* end(cstr const& s) { return s.end(); }
  void assign(char const* s, size_t l) {
    str = s;
    len = l;
  }
  cstr substr(size_t position, size s) const {
    auto const adjusted_position = std::min(position, len);
    auto const adjusted_size = std::min(s.size_, len - adjusted_position);
    return {str + adjusted_position, adjusted_size};
  }
  cstr substr(size_t begin, size_t end) const {
    auto const adjusted_begin = std::min(begin, len);
    auto const adjusted_end = std::min(end, len);
    return {str + adjusted_begin, adjusted_end - adjusted_begin};
  }
  cstr substr(size_t begin) const { return {str + begin, len - begin}; }
  cstr substr(field const& f) const {
    return (f.size == field::MAX_SIZE) ? substr(f.from)
                                       : substr(f.from, size(f.size));
  }
  bool contains(cstr needle) const {
    return view().find(needle.view()) != std::string_view::npos;
  }
  bool starts_with(cstr prefix) const {
    if (len < prefix.len) {
      return false;
    }
    return substr(0, size(prefix.len)) == prefix;
  }
  static bool is_space(char const c) { return c == ' '; }
  cstr skip_whitespace_front() const {
    auto copy = (*this);
    while (copy.len != 0 && is_space(copy[0])) {
      ++copy;
    }
    return copy;
  }
  cstr skip_whitespace_back() const {
    auto copy = (*this);
    while (copy.len != 0 && is_space(copy.str[copy.len - 1])) {
      --copy.len;
    }
    return copy;
  }
  cstr trim() const { return skip_whitespace_front().skip_whitespace_back(); }
  bool empty() const { return len == 0; }
  size_t length() const { return len; }
  char const* c_str() const { return str; }
  char const* data() const { return str; }
  size_t substr_offset(cstr needle) {
    for (size_t i = 0; i < len; ++i) {
      if (substr(i).starts_with(needle)) {
        return i;
      }
    }
    return std::numeric_limits<size_t>::max();
  }
  std::string to_str() const { return std::string(str, len); }
  std::string_view view() const { return {str, len}; }

  char const* str;
  size_t len;
};

inline cstr get_until(cstr s, char delimiter) {
  if (s.len == 0) {
    return s;
  }
  auto end = static_cast<char const*>(std::memchr(s.str, delimiter, s.len));
  if (end == nullptr) {
    return s;
  }
  return {s.str, static_cast<size_t>(end - s.str)};
}

inline cstr strip_cr(cstr s) {
  return (s && s[s.len - 1] == '\r') ? s.substr(0, size(s.len - 1)) : s;
}

inline cstr get_line(cstr s) { return strip_cr(get_until(s, '\n')); }

template <typename Function>
void for_each_token(cstr s, char separator, Function&& f) {
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

struct line_iterator {
  using iterator_category = std::input_iterator_tag;
  using value_type = cstr;
  using difference_type = std::ptrdiff_t;
  using pointer = cstr;
  using reference = cstr;

  line_iterator() = default;
  explicit line_iterator(cstr s) : s_{s} { ++*this; }

  explicit operator bool() const { return s_; }

  bool operator==(line_iterator const& i) const {
    return line_.data() == i.line_.data() && s_ == i.s_;
  }
  bool operator!=(line_iterator const& i) const { return !(*this == i); }

  line_iterator& operator+=(difference_type n) {
    for (int i = 0; i != n; ++i) {
      ++*this;
    }
    return *this;
  }
  line_iterator& operator++() {
    line_ = get_until(s_, '\n');
    s_ += line_.len;
    if (s_.len != 0) {
      ++s_;  // skip separator
    }
    line_ = strip_cr(line_);
    if (line_.begin() == s_.end()) {
      line_ = cstr{};
    }
    return *this;
  }
  line_iterator operator++(int) {
    line_iterator curr = *this;
    ++*this;
    return curr;
  }
  line_iterator operator+(difference_type const n) {
    line_iterator curr = *this;
    *this += n;
    return curr;
  }

  reference operator*() { return line_; }
  reference const& operator*() const { return line_; }
  pointer* operator->() { return &line_; }

  cstr s_;
  cstr line_;
};

struct lines {
  explicit lines(cstr s) : s_{s} {}
  line_iterator begin() const { return line_iterator{s_}; }
  line_iterator end() const { return line_iterator{cstr{}}; }
  cstr s_;
};

template <typename Function>
void for_each_token_numbered(cstr s, char separator, Function&& f) {
  int token_number = 0;
  for_each_token(s, separator, [&](cstr token) { f(token, ++token_number); });
}

template <typename Function>
void for_each_line(cstr s, Function&& f) {
  for_each_token(s, '\n', [&f](cstr token) { f(strip_cr(token)); });
}

template <typename Function>
void for_each_line_numbered(cstr s, Function&& f) {
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
