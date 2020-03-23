#pragma once

#include <iterator>
#include <utility>

namespace utl {

// precondition everywhere: ranges a and b should be sorted
template <typename ItA, typename ItB, typename Less, typename FnMatch,
          typename FnA, typename FnB>
void join_impl(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
               FnMatch&& fn_match, FnA&& fn_a, FnB&& fn_b) {
  using std::next;

  auto a_lower = a_begin;
  auto b_lower = b_begin;

  while (a_lower != a_end) {
    auto a_upper = a_lower;
    while (a_upper != a_end && !(less(*a_lower, *a_upper))) {
      ++a_upper;
    }

    auto b_start = b_lower;
    while (b_lower != b_end && less(*b_lower, *a_lower)) {
      if (next(b_lower) == b_end || less(*b_lower, *next(b_lower))) {
        fn_b(b_start, next(b_lower));
        b_start = next(b_lower);
      }

      ++b_lower;
    }

    if (b_lower == b_end) {
      if (a_lower != a_end) {
        fn_a(a_lower, a_upper);
        a_lower = a_upper;
        continue;  // finish "a" groups
      }
      break;  // both finished
    }
    if (less(*a_lower, *b_lower)) {
      fn_a(a_lower, a_upper);
      a_lower = a_upper;
      continue;  // no b for this a found
    }

    auto b_upper = b_lower;
    while (b_upper != b_end && !less(*b_lower, *b_upper)) {
      ++b_upper;
    }

    fn_match(a_lower, a_upper, b_lower, b_upper);
    a_lower = a_upper;
    b_lower = b_upper;
  }

  auto b_last = b_lower;
  while (b_last != b_end) {  // finish remaining b groups
    if (next(b_last) == b_end || less(*b_last, *next(b_last))) {
      fn_b(b_lower, next(b_last));
      b_lower = next(b_last);
    }
    ++b_last;
  }
}

// --- inner join
template <typename ItA, typename ItB, typename Less, typename FnMatch>
void inner_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
                FnMatch&& fn_match) {
  join_impl(
      a_begin, a_end, b_begin, b_end, std::forward<Less>(less),
      std::forward<FnMatch>(fn_match), [](auto, auto) {}, [](auto, auto) {});
}

template <typename ContainerA, typename ContainerB, typename Less,
          typename FnMatch>
void inner_join(ContainerA&& a, ContainerB&& b, Less&& less,
                FnMatch&& fn_match) {
  inner_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
             std::forward<Less>(less), std::forward<FnMatch>(fn_match));
}

template <typename ItA, typename ItB, typename FnMatch>
void inner_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end,
                FnMatch&& fn_match) {
  join_impl(
      a_begin, a_end, b_begin, b_end,
      [](auto const& lhs, auto const& rhs) { return lhs < rhs; },
      std::forward<FnMatch>(fn_match), [](auto, auto) {}, [](auto, auto) {});
}

template <typename ContainerA, typename ContainerB, typename FnMatch>
void inner_join(ContainerA&& a, ContainerB&& b, FnMatch&& fn_match) {
  inner_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
             std::forward<FnMatch>(fn_match));
}

// --- left outer join
template <typename ItA, typename ItB, typename Less, typename FnMatch,
          typename FnLeft>
void left_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
               FnMatch&& fn_match, FnLeft&& fn_left) {
  join_impl(a_begin, a_end, b_begin, b_end, std::forward<Less>(less),
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left),
            [](auto, auto) {});
}

template <typename ContainerA, typename ContainerB, typename Less,
          typename FnMatch, typename FnLeft>
void left_join(ContainerA&& a, ContainerB&& b, Less&& less, FnMatch&& fn_match,
               FnLeft&& fn_left) {
  left_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
            std::forward<Less>(less), std::forward<FnMatch>(fn_match),
            std::forward<FnLeft>(fn_left));
}

template <typename ItA, typename ItB, typename FnMatch, typename FnLeft>
void left_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end,
               FnMatch&& fn_match, FnLeft&& fn_left) {
  join_impl(
      a_begin, a_end, b_begin, b_end,
      [](auto const& lhs, auto const& rhs) { return lhs < rhs; },
      std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left),
      [](auto, auto) {});
}

template <typename ContainerA, typename ContainerB, typename FnMatch,
          typename FnLeft>
void left_join(ContainerA&& a, ContainerB&& b, FnMatch&& fn_match,
               FnLeft&& fn_left) {
  left_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left));
}

// -- full outer join

template <typename ItA, typename ItB, typename Less, typename FnMatch,
          typename FnLeft, typename FnRight>
void full_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
               FnMatch&& fn_match, FnLeft&& fn_left, FnRight&& fn_right) {
  join_impl(a_begin, a_end, b_begin, b_end, std::forward<Less>(less),
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left),
            std::forward<FnRight>(fn_right));
}

template <typename ContainerA, typename ContainerB, typename Less,
          typename FnMatch, typename FnLeft, typename FnRight>
void full_join(ContainerA&& a, ContainerB&& b, Less&& less, FnMatch&& fn_match,
               FnLeft&& fn_left, FnRight&& fn_right) {
  full_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
            std::forward<Less>(less), std::forward<FnMatch>(fn_match),
            std::forward<FnLeft>(fn_left), std::forward<FnRight>(fn_right));
}

template <typename ItA, typename ItB, typename FnMatch, typename FnLeft,
          typename FnRight>
void full_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end,
               FnMatch&& fn_match, FnLeft&& fn_left, FnRight&& fn_right) {
  join_impl(
      a_begin, a_end, b_begin, b_end,
      [](auto const& lhs, auto const& rhs) { return lhs < rhs; },
      std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left),
      std::forward<FnRight>(fn_right));
}

template <typename ContainerA, typename ContainerB, typename FnMatch,
          typename FnLeft, typename FnRight>
void full_join(ContainerA&& a, ContainerB&& b, FnMatch&& fn_match,
               FnLeft&& fn_left, FnRight&& fn_right) {
  full_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left),
            std::forward<FnRight>(fn_right));
}

template <typename Key>
struct join_less {
  template <typename Lhs, typename Rhs>
  bool operator()(Lhs const& lhs, Rhs const& rhs) const {
    return Key{}.key(lhs) < Key{}.key(rhs);
  }
};

}  // namespace utl
