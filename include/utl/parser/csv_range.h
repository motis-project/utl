#pragma once

#include <array>
#include <optional>

#include "cista/reflection/for_each_field.h"

#include "utl/const_str.h"
#include "utl/parser/arg_parser.h"
#include "utl/parser/buf_reader.h"
#include "utl/parser/csv.h"
#include "utl/parser/line_range.h"
#include "utl/pipes/all.h"
#include "utl/pipes/for_each.h"
#include "utl/pipes/is_range.h"

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
  T const* operator->() const { return &t; }
  T const& operator*() const { return t; }
  T& operator*() { return t; }
  T& val() { return t; }
  T t{};
};

template <typename T>
constexpr auto get_name(T) {
  return T::name_;
}

template <typename T, char Separator = ','>
std::array<column_idx_t, MAX_COLUMNS> read_header(cstr s) {
  if (s.starts_with("\xEF\xBB\xBF")) {
    // skip utf-8 byte order mark (otherwise the first column is ignored)
    s = s.substr(3);
  }

  std::array<column_idx_t, MAX_COLUMNS> column_map;
  column_map.fill(NO_COLUMN_IDX);

  for (column_idx_t column = 0; s; ++column) {
    cstr header;
    parse_column<cstr, Separator>(s, header);

    utl_verify(column < MAX_COLUMNS, "too many columns: {} >= {}", column,
               MAX_COLUMNS);
    column_map[column] = NO_COLUMN_IDX;
    column_idx_t c = 0u;
    cista::for_each_field<T>([&](auto&& f) {
      if (header.trim() == get_name(f)) {
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

template <typename T, char Separator = ','>
T read_row(std::array<column_idx_t, MAX_COLUMNS> const& headers_permutation,
           cstr s) {
  std::array<cstr, MAX_COLUMNS> row;
  for (column_idx_t i = 0; i < MAX_COLUMNS && s; ++i) {
    cstr column_content;
    parse_column<cstr, Separator>(s, column_content);

    if (headers_permutation[i] != NO_COLUMN_IDX) {
      row[headers_permutation[i]] = column_content;
    }

    if (s) {
      ++s;
    }
  }

  T t{};
  cista::for_each_field(t, [&, i = 0u](auto& f) mutable {
    if (row[i]) {
      parse_value(row[i], f.val());
    }
    ++i;
  });
  return t;
}

template <typename T, typename LineRange, char Separator = ','>
struct csv_range : public LineRange {
  using result_t = T;

  csv_range(LineRange&& r)
      : LineRange{std::forward<LineRange>(r)},
        headers_permutation_{read_header<T, Separator>(LineRange::begin())} {}

  std::optional<T> begin() {
    cstr s;
    LineRange::next(s);
    if (LineRange::valid(s)) {
      return read_row<T, Separator>(headers_permutation_, s);
    } else {
      return std::nullopt;
    }
  }

  template <typename It>
  auto&& read(It& it) const {
    return *it;
  }

  template <typename It>
  void next(It& it) {
    cstr s;
    LineRange::next(s);
    if (LineRange::valid(s)) {
      it = read_row<T, Separator>(headers_permutation_, s);
    } else {
      it = std::nullopt;
    }
  }

  template <typename It>
  bool valid(It& it) const {
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

template <typename T, typename Fn>
void for_each_row(std::string_view file_content, Fn&& fn) {
  line_range{utl::make_buf_reader(file_content)}  //
      | csv<T>()  //
      | for_each(std::forward<Fn>(fn));
}

}  // namespace utl
