#include <string>

#include "utl/struct/for_each_member.h"

#include "string_literal.h"

template <char const* str>
struct col_name {
  static constexpr auto const name_ = str;
};
#define name(str) col_name<STRING_LITERAL(str)>

struct primary {};
template <typename T>
bool is_primary(T) {
  return std::is_base_of_v<primary, T>;
}

struct not_null {};

template <typename T>
bool is_not_null(T) {
  return std::is_base_of_v<not_null, T>;
}

template <typename T, typename... Tags>
struct sql_col : Tags... {
  sql_col() = default;
  sql_col(T t) : t(t) {}
  operator T() { return t; }
  T& val() { return t; }
  T t;
};

template <typename T>
constexpr auto get_name(T) {
  return T::name_;
}

template <typename T>
constexpr auto col_type_str(T) {
  if constexpr (std::is_same_v<T, int>) {
    return "INT";
  } else if (std::is_same_v<T, std::string>) {
    return "TEXT";
  } else {
    throw std::runtime_error("unkown row type");
  }
}

template <typename T>
std::string create_table_statement(T r) {
  std::string s;
  bool first = true;
  s += "CREATE TABLE (";
  utl::for_each_field(r, [&](auto&& col) {
    s += (first ? "\n  " : ",\n  ");
    first = false;

    s += std::string(col.name_) + " " + col_type_str(col.val()) + " " +
         (is_primary(col) ? "PRIMARY " : "") +
         (is_not_null(col) ? "NOT NULL" : "");
  });
  s += "\n);";
  return s;
}