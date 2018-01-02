#pragma once

#include <ostream>

#include "utl/struct/for_each_field.h"

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
