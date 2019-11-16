#pragma once

#include <array>
#include <optional>

#include "utl/const_str.h"
#include "utl/parser/arg_parser.h"
#include "utl/parser/csv.h"
#include "utl/pipes/all.h"
#include "utl/pipes/is_range.h"
#include "utl/struct/for_each_field.h"

namespace utl {

template <char const* str>
struct csv_col_name {
  static constexpr auto const name_ = str;
};
#define UTL_NAME(str) utl::csv_col_name<STRING_LITERAL(str)>

template <typename T, typename... Tags>
struct csv_col : Tags... {
  csv_col() = default;
  csv_col(T param) : t(param) {}
  operator T() { return t; }
  T const& val() const { return t; }
  T& val() { return t; }
  T t{};
};

template <typename T>
constexpr auto get_name(T) {
  return T::name_;
}

template <typename T, char Separator = ','>
std::array<column_idx_t, MAX_COLUMNS> read_header(cstr s) {
  std::array<column_idx_t, MAX_COLUMNS> column_map;
  column_map.fill(NO_COLUMN_IDX);

  for (column_idx_t column = 0; s; ++column) {
    cstr header;
    parse_column<cstr, Separator>(s, header);

    column_map[column] = NO_COLUMN_IDX;
    column_idx_t c = 0u;
    for_each_field<T>([&](auto&& f) {
      if (header == get_name(f)) {
        column_map[column] = c;
        return;
      }
      ++c;
    });

    if (s) {
      ++s;
    }
  }

  return column_map;
}

template <typename T, typename LineRange, char Separator = ','>
struct csv_range : public LineRange {
  csv_range(LineRange&& r)
      : LineRange{std::forward<LineRange>(r)},
        headers_permutation_{read_header<T>(LineRange::begin())} {}

  inline T read_row(cstr s) {
    std::array<cstr, MAX_COLUMNS> row;
    for (column_idx_t i = 0; i < MAX_COLUMNS && s; ++i) {
      cstr column_content;
      parse_column<cstr, Separator>(s, column_content);

      if (headers_permutation_[i] != NO_COLUMN_IDX) {
        row[headers_permutation_[i]] = column_content;
      }

      if (s) {
        ++s;
      }
    }

    T t{};
    for_each_field(t, [&, i = 0u](auto& f) mutable {
      if (row[i]) {
        parse_arg(row[i], f.val());
      }
      ++i;
    });
    return t;
  }

  auto begin() { return std::make_optional<T>(read_row(LineRange::begin())); }

  template <typename It>
  auto&& read(It& it) {
    return *it;
  }

  template <typename It>
  void next(It& it) {
    cstr s;
    LineRange::next(s);
    if (LineRange::valid(s)) {
      it = read_row(s);
    } else {
      it = std::nullopt;
    }
  }

  template <typename It>
  bool valid(It& it) {
    return it.has_value();
  }

  std::array<column_idx_t, MAX_COLUMNS> headers_permutation_;
};

template <typename T, char Separator = ','>
struct csv {
  template <typename LineRange>
  friend auto operator|(LineRange&& r, csv&&) {
    return csv_range<T, LineRange, Separator>{std::forward<LineRange>(r)};
  }
};

template <typename T, typename LineRange, char Separator>
struct is_range<csv_range<T, LineRange, Separator>> : std::true_type {};

}  // namespace utl