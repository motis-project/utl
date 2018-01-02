#pragma once

#include <tuple>

#include "utl/struct/arity.h"

namespace utl {

template <typename T>
inline auto to_tuple(T&, size<0>) {
  return std::tie();
}

template <typename T>
inline auto to_tuple(T& t, size<1>) {
  auto & [p1] = t;
  return std::tie(p1);
}

template <typename T>
inline auto to_tuple(T& t, size<2>) {
  auto & [ p1, p2 ] = t;
  return std::tie(p1, p2);
}

template <typename T>
inline auto to_tuple(T& t, size<3>) {
  auto & [ p1, p2, p3 ] = t;
  return std::tie(p1, p2, p3);
}

template <typename T>
inline auto to_tuple(T& t, size<4>) {
  auto & [ p1, p2, p3, p4 ] = t;
  return std::tie(p1, p2, p3, p4);
}

template <typename T>
inline auto to_tuple(T& t, size<5>) {
  auto & [ p1, p2, p3, p4, p5 ] = t;
  return std::tie(p1, p2, p3, p4, p5);
}

template <typename T>
inline auto to_tuple(T& t, size<6>) {
  auto & [ p1, p2, p3, p4, p5, p6 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6);
}

template <typename T>
inline auto to_tuple(T& t, size<7>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7);
}

template <typename T>
inline auto to_tuple(T& t, size<8>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7, p8 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7, p8);
}

template <typename T>
inline auto to_tuple(T& t, size<9>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7, p8, p9 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9);
}

template <typename T>
inline auto to_tuple(T& t, size<10>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7, p8, p9, p10 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
}

template <typename T>
inline auto to_tuple(T& t, size<11>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
}

template <typename T>
inline auto to_tuple(T& t, size<12>) {
  auto & [ p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12 ] = t;
  return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
}

template <typename T>
inline auto to_tuple(T& t) {
  return to_tuple(t, arity(t));
}

}  // namespace utl
