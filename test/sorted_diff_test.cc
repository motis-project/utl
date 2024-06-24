#include "gtest/gtest.h"

#include <sstream>

#include "utl/overloaded.h"
#include "utl/sorted_diff.h"

using namespace utl;

namespace {

struct test {
  friend std::ostream& operator<<(std::ostream& out, test const& t) {
    return out << "(c=" << t.c_ << ", i=" << t.i_ << ")";
  }
  int c_, i_;
};

}  // namespace

TEST(sorted_diff, simple) {
  auto ss = std::stringstream{};
  sorted_diff(
      std::vector<test>{
          {.c_ = 1, .i_ = 7}, {.c_ = 2, .i_ = 7}, {.c_ = 3, .i_ = 7}},
      std::vector<test>{
          {.c_ = 2, .i_ = 7}, {.c_ = 3, .i_ = 8}, {.c_ = 4, .i_ = 7}},
      [](auto&& a, auto&& b) { return a.c_ < b.c_; },
      [](auto&& a, auto&& b) { return a.i_ == b.i_; },
      overloaded{[&](op const o, test const& t) {
                   ss << to_str(o) << ": " << t << "\n";
                 },
                 [&](test const& a, test const& b) {
                   ss << a << " -> " << b << "\n";
                 }});

  EXPECT_EQ(R"(del: (c=1, i=7)
(c=3, i=7) -> (c=3, i=8)
add: (c=4, i=7)
)",
            ss.str());
}