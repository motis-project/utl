#pragma once

#include <cassert>
#include <filesystem>
#include <string>

#include "utl/parser/cstr.h"
#include "utl/verify.h"

namespace utl {

template <typename IntType>
inline bool parse_arg(
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

  return value_okay && s.empty();
}

template <typename T>
inline bool parse_fp(cstr& s, T& v) {
  if (!s) {
    v = {};
    return false;
  }

  auto sign = T{1.0};
  if (*s.str == '-') {
    sign = -1.0;
    ++s;
  }

  int pre_point;
  parse_arg(s, pre_point);
  v = static_cast<T>(pre_point);
  if (s.len == 0) {
    v *= sign;
    return true;
  }

  if (*s.str != '.') {
    v *= sign;
    return false;
  }
  ++s;

  auto is_good = true;
  T multiplier = static_cast<T>(0.1);
  while (s) {
    char c = *s.str;
    if (c > '9' || c < '0') {
      is_good = false;
      break;
    }
    v += multiplier * (c - '0');
    multiplier *= static_cast<T>(0.1);
    if (s) {
      ++s;
    }
  }
  v *= sign;

  return is_good;
}

inline bool parse_arg(cstr& s, float& arg) { return parse_fp(s, arg); }

inline bool parse_arg(cstr& s, double& arg) { return parse_fp(s, arg); }

inline bool parse_arg(cstr& s, bool& b) {
  b = false;

  if (!s) {
    return false;
  }

  auto const c = std::tolower(*s.str);
  if (c == 't') {
    b = true;
    auto const ret = s.len == 4 &&  //
                     std::tolower(s[1]) == 'r' &&  //
                     std::tolower(s[2]) == 'u' &&  //
                     std::tolower(s[3]) == 'e';
    for (int i = 0; i < 4 && s; ++i, ++s)
      ;
    return ret;
  } else if (c == 'f') {
    auto const ret = s.len == 5 &&  //
                     std::tolower(s[1]) == 'a' &&  //
                     std::tolower(s[2]) == 'l' &&  //
                     std::tolower(s[3]) == 's' &&  //
                     std::tolower(s[4]) == 'e';
    for (int i = 0; i < 5 && s; ++i, ++s)
      ;
    return ret;
  }
  return false;
}

inline void parse_arg(cstr& s, std::string& arg) { arg.assign(s.str, s.len); }

inline void parse_arg(cstr& s, cstr& arg) { arg.assign(s.str, s.len); }

inline void parse_arg(cstr& s, std::filesystem::path& arg) { arg = s.str; }

template <typename T>
inline T parse(cstr s) {
  T value;
  parse_arg(s, value);
  return value;
}

template <typename T>
inline T parse_verify(cstr s) {
  T value;
  auto const initial = s;
  auto const is_successful = parse_arg(s, value);
  utl::verify(is_successful, "could not read {}", initial.view());
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
