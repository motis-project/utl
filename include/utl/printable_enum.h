#include <algorithm>
#include <iostream>
#include <string>

#define PRINTABLE_ENUM(name, ...)                                    \
  enum class name { __VA_ARGS__ };                                   \
  inline std::string as_string(name const v) {                       \
    std::string s = #__VA_ARGS__;                                    \
    auto c = begin(s);                                               \
    auto start = begin(s);                                           \
    for (auto i = 0u; i <= static_cast<unsigned>(v); ++i) {          \
      while (c != end(s) && (*c == ' ' || *c == ',')) {              \
        ++c;                                                         \
      }                                                              \
      start = c;                                                     \
      c = std::find(std::next(c), end(s), ',');                      \
    }                                                                \
    return {start, c};                                               \
  }                                                                  \
  inline std::ostream& operator<<(std::ostream& out, name const v) { \
    return out << as_string(v);                                      \
  }
