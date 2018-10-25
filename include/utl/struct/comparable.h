#pragma once

#include "utl/struct/to_tuple.h"

#define MAKE_COMPARABLE()                            \
  template <typename T>                              \
  bool operator==(T&& b) const {                     \
    return utl::to_tuple(*this) == utl::to_tuple(b); \
  }                                                  \
                                                     \
  template <typename T>                              \
  bool operator!=(T&& b) const {                     \
    return utl::to_tuple(*this) != utl::to_tuple(b); \
  }                                                  \
                                                     \
  template <typename T>                              \
  bool operator<(T&& b) const {                      \
    return utl::to_tuple(*this) < utl::to_tuple(b);  \
  }                                                  \
                                                     \
  template <typename T>                              \
  bool operator<=(T&& b) const {                     \
    return utl::to_tuple(*this) <= utl::to_tuple(b); \
  }                                                  \
                                                     \
  template <typename T>                              \
  bool operator>(T&& b) const {                      \
    return utl::to_tuple(*this) > utl::to_tuple(b);  \
  }                                                  \
                                                     \
  template <typename T>                              \
  bool operator>=(T&& b) const {                     \
    return utl::to_tuple(*this) >= utl::to_tuple(b); \
  }
