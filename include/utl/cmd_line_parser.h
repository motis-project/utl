#pragma once

#include <iomanip>
#include <sstream>

#include "utl/const_str.h"
#include "utl/parser/arg_parser.h"
#include "utl/parser/cstr.h"
#include "utl/struct/for_each_field.h"

namespace utl {

struct required {};

template <char const* str>
struct cmd_line_flag_desc {
  static constexpr auto const desc_ = str;
};
#define UTL_DESC(str) utl::cmd_line_flag_desc<STRING_LITERAL(str)>

template <char const* str>
struct cmd_line_flag_short {
  static constexpr auto const short_ = str;
};
#define UTL_SHORT(str) utl::cmd_line_flag_short<STRING_LITERAL(str)>

template <typename T>
constexpr bool has_short_flag(decltype((void)T().short_, 0)) {
  return true;
}

template <typename T>
constexpr bool has_short_flag(...) {
  return false;
}

template <char const* str>
struct cmd_line_flag_long {
  static constexpr auto const long_ = str;
};
#define UTL_LONG(str) utl::cmd_line_flag_long<STRING_LITERAL(str)>

template <typename T>
constexpr bool has_long_flag(decltype((void)T().long_, 0)) {
  return true;
}

template <typename T>
constexpr bool has_long_flag(...) {
  return false;
}

template <typename T, typename... Tags>
struct cmd_line_flag : public Tags... {
  cmd_line_flag(T init) : t{std::forward<T>(init)} {}
  cmd_line_flag() = default;
  operator T() { return t; }
  T& val() { return t; }
  T const& val() const { return t; }
  T t{};
};

template <typename T>
inline T parse(int argc, char const** argv) {
  auto parse_flag = [](auto& val, cstr next_arg) {
    using FieldType = std::remove_cv_t<std::remove_reference_t<decltype(val)>>;
    if constexpr (std::is_same_v<FieldType, bool>) {
      val = true;
      return true;
    } else if (next_arg) {
      parse_arg(next_arg, val);
      return true;
    }
    return false;
  };

  T t{};
  utl::for_each_field(t, [&](auto& f) {
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(f)>>;

    auto found = false;
    for (auto i = 0; i < argc; ++i) {
      auto const s = cstr(argv[i]);

      if constexpr (has_short_flag<Type>(0)) {
        if (s == f.short_) {
          found = parse_flag(f.val(), i >= argc - 1 ? nullptr : argv[i + 1]);
        }
      }

      if constexpr (has_long_flag<Type>(0)) {
        if (s == f.long_) {
          found = parse_flag(f.val(), i >= argc - 1 ? nullptr : argv[i + 1]);
        }
      }
    }

    if (!found && std::is_base_of_v<required, Type>) {
      if constexpr (has_long_flag<Type>(0)) {
        throw std::runtime_error(f.long_);
      } else {
        throw std::runtime_error(f.short_);
      }
    }
  });
  return t;
}

template <typename T>
inline std::string description() {
  std::stringstream ss;
  T t{};
  auto first = true;
  utl::for_each_field(t, [&](auto& f) {
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(f)>>;

    if (first) {
      first = false;
    } else {
      ss << "\n";
    }

    ss << "  ";
    if constexpr (has_long_flag<Type>(0)) {
      ss << std::setw(14) << f.long_ << " ";
    }
    if constexpr (has_short_flag<Type>(0)) {
      ss << f.short_;
    } else {
      ss << "  ";
    }
    if constexpr (std::is_base_of_v<required, Type>) {
      ss << "    *";
    } else {
      ss << "     ";
    }

    ss << " " << f.desc_;
  });
  return ss.str();
}

template <typename T>
inline std::string print_flags(T const& t) {
  std::stringstream ss;
  auto first = true;
  utl::for_each_field(t, [&](auto& f) {
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(f)>>;
    using ValType =
        std::remove_cv_t<std::remove_reference_t<decltype(f.val())>>;

    if (first) {
      first = false;
    } else {
      ss << "\n";
    }

    ss << "  ";
    if constexpr (has_long_flag<Type>(0)) {
      ss << std::setw(14) << f.long_ << " ";
    }
    if constexpr (has_short_flag<Type>(0)) {
      ss << f.short_;
    } else {
      ss << "  ";
    }
    if constexpr (std::is_base_of_v<required, Type>) {
      ss << "    *";
    } else {
      ss << "     ";
    }

    ss << " ";
    if (std::is_same_v<ValType, bool>) {
      ss << std::boolalpha << f.val();
    } else {
      ss << f.val();
    }
  });
  return ss.str();
}

}  // namespace utl