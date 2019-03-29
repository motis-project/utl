#pragma once

namespace utl {

// precondition everywhere: ranges a and b should be sorted
template <typename ItA, typename ItB, typename Less, typename FnMatch,
          typename FnLeft>
void join_impl(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
               FnMatch&& fn_match, FnLeft&& fn_left) {
  auto a_lower = a_begin;
  auto b_lower = b_begin;

  while (a_lower != a_end) {
    auto a_upper = a_lower;
    while (a_upper != a_end && !(less(*a_lower, *a_upper))) {
      ++a_upper;
    }

    while (b_lower != b_end && less(*b_lower, *a_lower)) {
      ++b_lower;
    }

    if (b_lower == b_end) {
      if (a_lower != a_end) {
        fn_left(a_lower, a_upper);
        a_lower = a_upper;
        continue;  // finish "a" groups
      }
      break;  // both finished
    }
    if (less(*a_lower, *b_lower)) {
      fn_left(a_lower, a_upper);
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
}

// --- inner join
template <typename ItA, typename ItB, typename Less, typename FnMatch>
void inner_join(ItA a_begin, ItA a_end, ItB b_begin, ItB b_end, Less&& less,
                FnMatch&& fn_match) {
  join_impl(a_begin, a_end, b_begin, b_end, std::forward<Less>(less),
            std::forward<FnMatch>(fn_match), [](auto, auto) {});
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
  join_impl(a_begin, a_end, b_begin, b_end,
            [](auto const& lhs, auto const& rhs) { return lhs < rhs; },
            std::forward<FnMatch>(fn_match), [](auto, auto) {});
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
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left));
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
  join_impl(a_begin, a_end, b_begin, b_end,
            [](auto const& lhs, auto const& rhs) { return lhs < rhs; },
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left));
}

template <typename ContainerA, typename ContainerB, typename FnMatch,
          typename FnLeft>
void left_join(ContainerA&& a, ContainerB&& b, FnMatch&& fn_match,
               FnLeft&& fn_left) {
  left_join(std::begin(a), std::end(a), std::begin(b), std::end(b),
            std::forward<FnMatch>(fn_match), std::forward<FnLeft>(fn_left));
}

}  // namespace utl
