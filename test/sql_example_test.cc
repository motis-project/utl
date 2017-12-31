#include "catch.hpp"

#include <iostream>

#include "utl/struct/for_each_member.h"

#include "string_literal.h"

template <char const* str>
struct name {
  static constexpr auto const name_ = str;
};
struct primary {};
struct not_null {};

template <typename T, typename... Tags>
struct sql_field : Tags... {
  sql_field() = default;
  sql_field(T t) : t(t) {}
  operator T() { return t; }
  T& val() { return t; }
  T t;
};

struct a {
  sql_field<int, name<STRING_LITERAL("id")>, primary, not_null> user_id = 0;
  sql_field<std::string, name<STRING_LITERAL("firstName")>, not_null>
      first_name;
  sql_field<std::string, name<STRING_LITERAL("lastName")>, not_null> last_name;
};

template <typename T>
constexpr auto get_name(T) {
  return T::name_;
}

TEST_CASE("create_table") {
  a row;
  utl::for_each_field(row, [](auto&& col) { std::cout << col.val() << "\n"; });
}
