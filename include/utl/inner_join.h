#pragma once

namespace utl {

// precondition: ranges a and b should be sorted
template <typename ITA, typename ITB, typename Fn>
void inner_join(ITA a_begin, ITA a_end, ITB b_begin, ITB b_end, Fn&& fn) {
  auto a_lower = a_begin;
  auto b_lower = b_begin;

  while (a_lower != a_end) {
    auto a_upper = a_lower;
    while (a_upper != a_end && *a_lower == *a_upper) {
      ++a_upper;
    }

    while (b_lower != b_end && *b_lower < *a_lower) {
      ++b_lower;
    }

    if (b_lower == b_end) {
      break;  // no more matches possible
    }
    if (*b_lower > *a_lower) {
      a_lower = a_upper;
      continue;  // no b for this a found
    }

    auto b_upper = b_lower;
    while (b_upper != b_end && *b_lower == *b_upper) {
      ++b_upper;
    }

    fn(a_lower, a_upper, b_lower, b_upper);
    a_lower = a_upper;
    b_lower = b_upper;
  }
}

template <typename ContainerA, typename ContainerB, typename Fn>
void inner_join(ContainerA a, ContainerB b, Fn&& fn) {
  inner_join(std::begin(a), std::end(a), std::begin(b), std::end(b), fn);
}

}  // namespace utl
