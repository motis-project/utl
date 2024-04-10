#include "gtest/gtest.h"

#include <vector>

#include "utl/power_set_intersection.h"

namespace {

struct k {
  friend std::ostream& operator<<(std::ostream& out, k const& x) {
    return out << "(" << x.a_ << ", " << x.b_ << ")";
  }
  friend bool operator==(k const& a, k const& b) {
    return a.a_ == b.a_ && a.b_ == b.b_;
  }
  friend bool operator<(k const& a, k const& b) { return a.a_ < b.a_; }
  int a_, b_;
};

}  // namespace

TEST(power_set_intersection, basic) {
  auto intersection = std::vector<std::pair<k, k>>{};
  utl::power_set_intersection(
      std::vector<k>{{1, 100}, {1, 200}, {2, 300}, {3, 400}, {3, 500}},
      std::vector<k>{{1, 600}, {2, 700}, {2, 800}, {3, 900}, {3, 1000}},
      [&](auto&& x, auto&& y) { intersection.emplace_back(x, y); });

  auto const expected =
      std::vector<std::pair<k, k>>{{{1, 100}, {1, 600}}, {{1, 200}, {1, 600}},

                                   {{2, 300}, {2, 700}}, {{2, 300}, {2, 800}},

                                   {{3, 400}, {3, 900}}, {{3, 400}, {3, 1000}},
                                   {{3, 500}, {3, 900}}, {{3, 500}, {3, 1000}}};

  EXPECT_EQ(expected, intersection);
}