#pragma once

namespace utl {

template <char... Chars>
struct const_str {
  static constexpr char const s[sizeof...(Chars) + 1] = {Chars..., 0};
};

template <char... Chars>
constexpr char const const_str<Chars...>::s[sizeof...(Chars) + 1];

#define STRING_LITERAL_16(str) \
  STRING_LITERAL_15(str), ((TERMINATED_15(str)) ? (str[15]) : ('\0'))
#define STRING_LITERAL_15(str) \
  STRING_LITERAL_14(str), ((TERMINATED_14(str)) ? (str[14]) : ('\0'))
#define STRING_LITERAL_14(str) \
  STRING_LITERAL_13(str), ((TERMINATED_13(str)) ? (str[13]) : ('\0'))
#define STRING_LITERAL_13(str) \
  STRING_LITERAL_12(str), ((TERMINATED_12(str)) ? (str[12]) : ('\0'))
#define STRING_LITERAL_12(str) \
  STRING_LITERAL_11(str), ((TERMINATED_11(str)) ? (str[11]) : ('\0'))
#define STRING_LITERAL_11(str) \
  STRING_LITERAL_10(str), ((TERMINATED_10(str)) ? (str[10]) : ('\0'))
#define STRING_LITERAL_10(str) \
  STRING_LITERAL_9(str), ((TERMINATED_9(str)) ? (str[9]) : ('\0'))
#define STRING_LITERAL_9(str) \
  STRING_LITERAL_8(str), ((TERMINATED_8(str)) ? (str[8]) : ('\0'))
#define STRING_LITERAL_8(str) \
  STRING_LITERAL_7(str), ((TERMINATED_7(str)) ? (str[7]) : ('\0'))
#define STRING_LITERAL_7(str) \
  STRING_LITERAL_6(str), ((TERMINATED_6(str)) ? (str[6]) : ('\0'))
#define STRING_LITERAL_6(str) \
  STRING_LITERAL_5(str), ((TERMINATED_5(str)) ? (str[5]) : ('\0'))
#define STRING_LITERAL_5(str) \
  STRING_LITERAL_4(str), ((TERMINATED_4(str)) ? (str[4]) : ('\0'))
#define STRING_LITERAL_4(str) \
  STRING_LITERAL_3(str), ((TERMINATED_3(str)) ? (str[3]) : ('\0'))
#define STRING_LITERAL_3(str) \
  STRING_LITERAL_2(str), ((TERMINATED_2(str)) ? (str[2]) : ('\0'))
#define STRING_LITERAL_2(str) \
  STRING_LITERAL_1(str), ((TERMINATED_1(str)) ? (str[1]) : ('\0'))
#define STRING_LITERAL_1(str) str[0]
#define STRING_LITERAL(str) utl::const_str<STRING_LITERAL_16(str)>::s

#define TERMINATED_15(str) TERMINATED_14(str) && str[14]
#define TERMINATED_14(str) TERMINATED_13(str) && str[13]
#define TERMINATED_13(str) TERMINATED_12(str) && str[12]
#define TERMINATED_12(str) TERMINATED_11(str) && str[11]
#define TERMINATED_11(str) TERMINATED_10(str) && str[10]
#define TERMINATED_10(str) TERMINATED_9(str) && str[9]
#define TERMINATED_9(str) TERMINATED_8(str) && str[8]
#define TERMINATED_8(str) TERMINATED_7(str) && str[7]
#define TERMINATED_7(str) TERMINATED_6(str) && str[6]
#define TERMINATED_6(str) TERMINATED_5(str) && str[5]
#define TERMINATED_5(str) TERMINATED_4(str) && str[4]
#define TERMINATED_4(str) TERMINATED_3(str) && str[3]
#define TERMINATED_3(str) TERMINATED_2(str) && str[2]
#define TERMINATED_2(str) TERMINATED_1(str) && str[1]
#define TERMINATED_1(str) str[0]

}  // namespace utl