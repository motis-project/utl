#pragma once

#include "utl/const_str.h"
#include "utl/parser/arg_parser.h"
#include "utl/parser/cstr.h"
#include "utl/struct/for_each_field.h"

namespace utl {

template <char const* str>
struct cmd_line_flag_desc {
  static constexpr auto const desc_ = str;
};
#define UTL_DESC(str) cmd_line_flag_desc<STRING_LITERAL(str)>

template <char const* str>
struct cmd_line_flag_short {
  static constexpr auto const short_ = str;
};
#define UTL_SHORT(str) cmd_line_flag_short<STRING_LITERAL(str)>

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
#define UTL_LONG(str) cmd_line_flag_long<STRING_LITERAL(str)>

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
  cmd_line_flag() = default;
  cmd_line_flag(T t) : t(t) {}
  operator T() { return t; }
  T& val() { return t; }
  T t{};
};

template <typename T>
inline T parse(int argc, char const** argv) {
  auto parse_flag = [](auto& val, cstr next_arg) {
    using FieldType = std::remove_cv_t<std::remove_reference_t<decltype(val)>>;
    if constexpr (std::is_same_v<FieldType, bool>) {
      val = true;
    } else if (next_arg) {
      parse_arg(next_arg, val);
    }
  };

  T t{};
  utl::for_each_field(t, [&](auto& f) {
    using Type = std::remove_cv_t<std::remove_reference_t<decltype(f)>>;

    for (auto i = 0; i < argc; ++i) {
      auto const s = cstr(argv[i]);

      if constexpr (has_short_flag<Type>(0)) {
        if (s == f.short_) {
          parse_flag(f.val(), i == argc - 1 ? nullptr : argv[i + 1]);
        }
      }

      if constexpr (has_long_flag<Type>(0)) {
        if (s == f.long_) {
          parse_flag(f.val(), i == argc - 1 ? nullptr : argv[i + 1]);
        }
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
      ss << std::setw(16) << f.long_ << " ";
    }
    if constexpr (has_short_flag<Type>(0)) {
      ss << f.short_;
    } else {
      ss << "  ";
    }
    ss << "    " << f.desc_;
  });
  return ss.str();
}

}  // namespace utl