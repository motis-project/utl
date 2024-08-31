#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace utl {

namespace detail {

template <std::size_t N>
using field_count = std::integral_constant<std::size_t, N>;

struct wildcard {
  template <typename T,
            typename = std::enable_if_t<!std::is_lvalue_reference<T>::value>>
  operator T&&() const;

  template <typename T,
            typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
  operator T&() const;
};

template <std::size_t N = 0>
static constexpr const wildcard _{};

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

template <typename T, std::size_t... I>
inline constexpr auto is_brace_constructible_(std::index_sequence<I...>, T*)
    -> decltype(T{_<I>...}, std::true_type{}) {
  return {};
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

template <std::size_t... I>
inline constexpr std::false_type is_brace_constructible_(
    std::index_sequence<I...>, ...) {
  return {};
}

template <typename T, std::size_t N>
constexpr auto is_brace_constructible()
    -> decltype(is_brace_constructible_(std::make_index_sequence<N>{},
                                        static_cast<T*>(nullptr))) {
  return {};
}

template <typename T, typename U>
struct is_paren_constructible_;

template <typename T, std::size_t... I>
struct is_paren_constructible_<T, std::index_sequence<I...>>
    : std::is_constructible<T, decltype(_<I>)...> {};

template <typename T, std::size_t N>
constexpr auto is_paren_constructible()
    -> is_paren_constructible_<T, std::make_index_sequence<N>> {
  return {};
}

template <typename T, std::size_t N>
inline constexpr auto const has_arity_v =
    is_brace_constructible<T, N>() && !is_brace_constructible<T, N + 1>();

#define UTL_MAKE_ARITY_FUNC(count)                                          \
  template <typename T, typename = std::enable_if_t<has_arity_v<T, count>>> \
  constexpr field_count<count> arity() {                                    \
    return {};                                                              \
  }

UTL_MAKE_ARITY_FUNC(0)
UTL_MAKE_ARITY_FUNC(1)
UTL_MAKE_ARITY_FUNC(2)
UTL_MAKE_ARITY_FUNC(3)
UTL_MAKE_ARITY_FUNC(4)
UTL_MAKE_ARITY_FUNC(5)
UTL_MAKE_ARITY_FUNC(6)
UTL_MAKE_ARITY_FUNC(7)
UTL_MAKE_ARITY_FUNC(8)
UTL_MAKE_ARITY_FUNC(9)
UTL_MAKE_ARITY_FUNC(10)
UTL_MAKE_ARITY_FUNC(11)
UTL_MAKE_ARITY_FUNC(12)
UTL_MAKE_ARITY_FUNC(13)
UTL_MAKE_ARITY_FUNC(14)
UTL_MAKE_ARITY_FUNC(15)
UTL_MAKE_ARITY_FUNC(16)
UTL_MAKE_ARITY_FUNC(17)
UTL_MAKE_ARITY_FUNC(18)
UTL_MAKE_ARITY_FUNC(19)
UTL_MAKE_ARITY_FUNC(20)
UTL_MAKE_ARITY_FUNC(21)
UTL_MAKE_ARITY_FUNC(22)
UTL_MAKE_ARITY_FUNC(23)
UTL_MAKE_ARITY_FUNC(24)
UTL_MAKE_ARITY_FUNC(25)
UTL_MAKE_ARITY_FUNC(26)
UTL_MAKE_ARITY_FUNC(27)
UTL_MAKE_ARITY_FUNC(28)
UTL_MAKE_ARITY_FUNC(29)
UTL_MAKE_ARITY_FUNC(30)
UTL_MAKE_ARITY_FUNC(31)

#undef UTL_MAKE_ARITY_FUNC

template <typename T>
auto to_tuple(T& t) {
  constexpr auto const a = arity<T>();
  static_assert(a <= 31U, "Max. supported members: 64");
  if constexpr (a == 0U) {
    return std::tie();
  } else if constexpr (a == 1U) {
    auto& [p1] = t;
    return std::tie(p1);
  } else if constexpr (a == 2U) {
    auto& [p1, p2] = t;
    return std::tie(p1, p2);
  } else if constexpr (a == 3U) {
    auto& [p1, p2, p3] = t;
    return std::tie(p1, p2, p3);
  } else if constexpr (a == 4U) {
    auto& [p1, p2, p3, p4] = t;
    return std::tie(p1, p2, p3, p4);
  } else if constexpr (a == 5U) {
    auto& [p1, p2, p3, p4, p5] = t;
    return std::tie(p1, p2, p3, p4, p5);
  } else if constexpr (a == 6U) {
    auto& [p1, p2, p3, p4, p5, p6] = t;
    return std::tie(p1, p2, p3, p4, p5, p6);
  } else if constexpr (a == 7U) {
    auto& [p1, p2, p3, p4, p5, p6, p7] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7);
  } else if constexpr (a == 8U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8);
  } else if constexpr (a == 9U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9);
  } else if constexpr (a == 10U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
  } else if constexpr (a == 11U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
  } else if constexpr (a == 12U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
  } else if constexpr (a == 13U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
  } else if constexpr (a == 14U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13,
                    p14);
  } else if constexpr (a == 15U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15] =
        t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15);
  } else if constexpr (a == 16U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16);
  } else if constexpr (a == 17U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17);
  } else if constexpr (a == 18U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18);
  } else if constexpr (a == 19U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19);
  } else if constexpr (a == 20U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20);
  } else if constexpr (a == 21U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21);
  } else if constexpr (a == 22U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22);
  } else if constexpr (a == 23U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23);
  } else if constexpr (a == 24U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
  } else if constexpr (a == 25U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
  } else if constexpr (a == 26U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
  } else if constexpr (a == 27U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
                    p27);
  } else if constexpr (a == 28U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
                    p27, p28);
  } else if constexpr (a == 29U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28,
           p29] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
                    p27, p28, p29);
  } else if constexpr (a == 30U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29,
           p30] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
                    p27, p28, p29, p30);
  } else if constexpr (a == 31U) {
    auto& [p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15,
           p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29,
           p30, p31] = t;
    return std::tie(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14,
                    p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26,
                    p27, p28, p29, p30, p31);
  }
}

template <typename T>
consteval T* null() noexcept {
  return static_cast<T*>(nullptr);
}

template <typename T>
concept Derefable = requires(T t) { *t; };

template <typename T>
struct derefable {
  static constexpr bool const value = Derefable<T>;
};

template <typename T>
using deref_t = decltype(*std::declval<T>());

template <typename T, typename U>
struct deref_same {
  static constexpr auto const value =
      std::is_same_v<T, std::decay_t<deref_t<U>>>;
};

template <typename T, typename S, std::size_t I = 0U>
bool c(S&& s) {
  using Src = std::decay_t<std::tuple_element_t<I, std::decay_t<S>>>;
  using Target = std::decay_t<T>;

  if constexpr (std::is_same_v<Target, Src>) {
    return true;
  } else if constexpr (std::conjunction_v<derefable<Src>,
                                          deref_same<Target, Src>>) {
    return std::get<I>(s) != nullptr;
  } else {
    return c<T, S, I + 1U>(std::forward<S>(s));
  }
}

template <typename T, typename S, std::size_t I = 0U>
T& m(S&& s) {
  using Src = std::decay_t<std::tuple_element_t<I, std::decay_t<S>>>;
  using Target = std::decay_t<T>;

  if constexpr (std::is_same_v<Target, Src>) {
    return std::get<I>(s);
  } else if constexpr (std::conjunction_v<derefable<Src>,
                                          deref_same<Target, Src>>) {
    return *std::get<I>(s);
  } else {
    return m<T, S, I + 1U>(std::forward<S>(s));
  }
}

template <typename T, typename S, std::size_t... I>
std::optional<T> init_from(S&& s, std::index_sequence<I...>) {
  using Tuple = decltype(to_tuple(std::declval<T&>()));
  if (!(c<std::tuple_element_t<I, Tuple>>(s) && ...)) {
    return std::nullopt;
  }
  return T(m<std::tuple_element_t<I, Tuple>>(s)...);
}

}  // namespace detail

template <typename T, typename S>
std::optional<T> init_from(S&& s) {
  return detail::init_from<T>(detail::to_tuple(s),
                              std::make_index_sequence<detail::arity<T>()>());
}

}  // namespace utl
