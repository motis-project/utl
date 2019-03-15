#pragma once

#include <ostream>
#include <vector>

#include "utl/struct/for_each_field.h"

#ifndef UTL_PRINTABLE_NO_VEC
template <typename T>
inline std::ostream& operator<<(std::ostream& out, std::vector<T> const& v) {
  out << "[\n  ";
  auto first = true;
  for (auto const& e : v) {
    if (!first) {
      out << ",\n  ";
    }
    out << e;
    first = false;
  }
  return out << "\n]";
}
#endif

#define MAKE_PRINTABLE(class_name)                                          \
  friend std::ostream& operator<<(std::ostream& out, class_name const& o) { \
    bool first = true;                                                      \
    out << "{";                                                             \
    utl::for_each_field(o, [&](auto&& f) {                                  \
      if (first) {                                                          \
        out << f;                                                           \
        first = false;                                                      \
      } else {                                                              \
        out << ", " << f;                                                   \
      }                                                                     \
    });                                                                     \
    return out << "}";                                                      \
  }
