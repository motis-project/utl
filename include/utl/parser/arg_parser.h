#pragma once

#include <cassert>
#include <string>

#include "utl/parser/cstr.h"

namespace utl {

template <typename IntType>
inline void parse_arg(
    cstr& s, IntType& arg,
    typename std::enable_if<std::is_integral<IntType>::value, IntType>::type
        default_value = 0) {
  IntType sign = 1;
  if constexpr (std::is_signed_v<IntType>) {
    if (s && s[0] == '-') {
      sign = -1;
      ++s;
    }
  }

  if (s && s[0] == '+') {
    ++s;
  }

  arg = 0;
  bool value_okay = false;
  while (s) {
    char c = *s.str;
    if (c > '9' || c < '0') {
      break;
    }
    arg = arg * 10 + static_cast<IntType>(c - '0');
    value_okay = true;
    ++s;
  }

  if (!value_okay) {
    arg = default_value;
  } else {
    arg *= sign;
  }
}

template <typename T>
inline void parse_fp(cstr& s, T& v) {
  if (!s) {
    v = {};
    return;
  }

  int sign = 1;
  if (*s.str == '-') {
    sign = -1;
    ++s;
  }

  int pre_point;
  parse_arg(s, pre_point);
  v = static_cast<T>(pre_point);
  if (s.len == 0) {
    v *= sign;
    return;
  }

  if (*s.str != '.') {
    v *= sign;
    return;
  }
  ++s;

  T multiplier = static_cast<T>(0.1);
  while (s) {
    char c = *s.str;
    if (c > '9' || c < '0') {
      break;
    }
    v += multiplier * (c - '0');
    multiplier *= static_cast<T>(0.1);
    if (s) {
      ++s;
    }
  }
  v *= sign;
}

inline void parse_arg(cstr& s, float& arg) { parse_fp(s, arg); }

inline void parse_arg(cstr& s, double& arg) { parse_fp(s, arg); }

inline void parse_arg(cstr& s, bool& b) {
  b = false;

  if (!s) {
    return;
  }

  auto const c = std::tolower(*s.str);
  if (c == 't') {
    b = true;
    for (int i = 0; i < 4 && s; ++i, ++s)
      ;
  } else if (c == 'f') {
    for (int i = 0; i < 5 && s; ++i, ++s)
      ;
  }
}

inline void parse_arg(cstr& s, std::string& arg) { arg.assign(s.str, s.len); }

inline void parse_arg(cstr& s, cstr& arg) { arg.assign(s.str, s.len); }

template <typename T>
inline T parse(cstr s) {
  T value;
  parse_arg(s, value);
  return value;
}

template <typename T>
inline T parse(cstr s, T default_value) {
  T value;
  parse_arg(s, value, default_value);
  return value;
}

template <typename T>
inline cstr& operator>>(cstr& s, T& i) {
  parse_arg(s, i);
  return s;
}

template <char C>
struct expect_char {};

template <char C>
inline cstr& operator>>(cstr& s, expect_char<C>) {
  bool nonempty = s.len != 0;
  assert(nonempty && *s.str == C);
  if (nonempty) {
    ++s;
  }
  return s;
}

inline void skip_line(cstr& s) {
  while (s && *s.str != '\n') {
    ++s;
  }
  if (s && *s.str == '\n') {
    ++s;
  }
}

}  // namespace utl
