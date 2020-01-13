#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <tuple>
#include <vector>

#include "utl/parser/arg_parser.h"
#include "utl/parser/cstr.h"
#include "utl/parser/file.h"

namespace utl {

template <typename T, char Separator = ','>
inline void parse_column(cstr& s, T& arg) {
  constexpr char const kEscapeChar = '\\';
  char end = Separator;
  if (*s.str == '"') {
    end = '"';
    ++s;
  }

  auto begin = s;
  int adjust_for_cr = 0;
  while (s) {
    char c = *s.str;

    // Skip escaped end character ("")
    if (end != Separator && c == kEscapeChar && s.len >= 2 && s.str[1] == end) {
      s += 2;
      continue;
    }

    if (c == end || c == '\r' || c == '\n' || s.len == 0) {
      if ((end == '"' && s.len != 0) || (c == '\r' && s.len != 0)) {
        ++s;
      }
      adjust_for_cr = (c == '\r') ? -1 : 0;
      break;
    }

    if (s) {
      ++s;
    }
  }

  int adjust_for_quote = (end == '"') ? -1 : 0;
  arg.assign(begin.str, static_cast<size_t>((s.str - begin.str) +
                                            adjust_for_quote + adjust_for_cr));
}

template <int Index, typename... Args>
typename std::enable_if<Index == sizeof...(Args)>::type read(
    std::tuple<Args...>&, std::array<cstr, sizeof...(Args)>&) {}

template <int Index, typename... Args>
typename std::enable_if<Index != sizeof...(Args)>::type read(
    std::tuple<Args...>& args, std::array<cstr, sizeof...(Args)>& row) {
  parse_arg(row[Index], std::get<Index>(args));
  read<Index + 1, Args...>(args, row);
}

template <typename... Args>
void read(std::tuple<Args...>& args, std::array<cstr, sizeof...(Args)>& row) {
  read<0, Args...>(args, row);
}

template <typename... Args>
void read(std::vector<std::tuple<Args...>>& v,
          std::vector<std::array<cstr, sizeof...(Args)>>& rows) {
  v.resize(rows.size());
  for (unsigned row_idx = 0; row_idx < rows.size(); ++row_idx) {
    read(v[row_idx], rows[row_idx]);
  }
}

typedef uint8_t column_idx_t;
constexpr column_idx_t NO_COLUMN_IDX = std::numeric_limits<column_idx_t>::max();
constexpr column_idx_t MAX_COLUMNS = 32;

template <typename Tuple, char Separator = ','>
std::array<column_idx_t, MAX_COLUMNS> read_header(
    cstr& s, std::array<cstr, std::tuple_size<Tuple>::value> defined_columns) {
  std::array<column_idx_t, MAX_COLUMNS> column_map;
  std::fill(std::begin(column_map), std::end(column_map), NO_COLUMN_IDX);

  for (column_idx_t column = 0; s && *s.str != '\r' && *s.str != '\n';
       ++column, ++s) {
    cstr header;
    parse_column<cstr, Separator>(s, header);

    column_map[column] = NO_COLUMN_IDX;
    for (column_idx_t c = 0; c != defined_columns.size(); ++c) {
      if (header == defined_columns[c]) {
        column_map[column] = c;
        break;
      }
    }

    if (s && (*s.str == '\r' || *s.str == '\n')) {
      break;
    }
  }

  skip_line(s);

  return column_map;
}

template <typename Tuple, char Separator = ','>
std::array<cstr, std::tuple_size<Tuple>::value> read_row(
    cstr& s, std::array<column_idx_t, MAX_COLUMNS> column_map,
    std::size_t last_column_index) {
  std::array<cstr, std::tuple_size<Tuple>::value> row;

  for (column_idx_t i = 0; i < last_column_index && s; ++i) {
    cstr column_content;
    parse_column<cstr, Separator>(s, column_content);

    if (column_map[i] != NO_COLUMN_IDX) {
      row[column_map[i]] = column_content;
    }

    if (s && *s.str != '\r' && *s.str != '\n') {
      ++s;
    }
  }

  skip_line(s);

  return row;
}

inline bool has_row_remaining(cstr s) {
  return s.len > 2 ||  // no special case possible
         (s.len == 2 && s[0] != '\r') ||  // "\r\n"
         (s.len == 1 && s[0] != '\n');  // "\n"
}

template <typename Tuple, char Separator = ','>
std::vector<std::array<cstr, std::tuple_size<Tuple>::value>> read_rows(
    cstr& s, std::array<column_idx_t, MAX_COLUMNS> column_map,
    std::size_t last_column_index) {
  std::vector<std::array<cstr, std::tuple_size<Tuple>::value>> rows;
  while (has_row_remaining(s)) {
    rows.push_back(
        read_row<Tuple, Separator>(s, column_map, last_column_index));
  }
  return rows;
}

template <typename Tuple, char Separator = ','>
std::vector<std::array<cstr, std::tuple_size<Tuple>::value>> read_rows(
    cstr& s, std::array<column_idx_t, MAX_COLUMNS> column_map) {
  return read_rows<Tuple, Separator>(
      s, column_map,
      static_cast<size_t>(std::distance(
          std::find_if_not(
              column_map.rbegin(), column_map.rend(),
              [](column_idx_t idx) { return idx == NO_COLUMN_IDX; }),
          column_map.rend())));
}

template <typename Tuple>
using column_mapping = std::array<cstr, std::tuple_size<Tuple>::value>;

template <typename Tuple, char Separator = ','>
void read(cstr s, std::vector<Tuple>& entries,
          column_mapping<Tuple> defined_columns) {
  auto column_map = read_header<Tuple, Separator>(s, defined_columns);
  auto rows = read_rows<Tuple, Separator>(s, column_map);
  read(entries, rows);
}

template <typename Tuple, char Separator = ','>
std::vector<Tuple> read(cstr s, column_mapping<Tuple> defined_columns) {
  auto column_map = read_header<Tuple, Separator>(s, defined_columns);
  auto rows = read_rows<Tuple, Separator>(s, column_map);
  std::vector<Tuple> entries;
  read(entries, rows);
  return entries;
}

template <typename Tuple, char Separator = ','>
void read_file(const char* path, std::vector<Tuple>& entries,
               column_mapping<Tuple> defined_columns) {
  auto buf = file(path, "ro").content();
  cstr s(buf.data(), buf.size());
  auto column_map = read_header<Tuple, Separator>(s, defined_columns);
  auto rows = read_rows<Tuple, Separator>(s, column_map);
  read(entries, rows);
}

template <typename Tuple, typename StrType, char Separator = ','>
std::vector<Tuple> read_file(StrType path,
                             column_mapping<Tuple> defined_columns) {
  std::vector<Tuple> vec;
  read_file<Tuple, Separator>(path.c_str(), vec, defined_columns);
  return vec;
}

}  // namespace utl
