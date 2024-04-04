#include "gtest/gtest.h"

#include "utl/match.h"

struct parent {
  parent() = default;
  parent(parent const&) = default;
  parent(parent&&) = default;
  parent& operator=(parent const&) = default;
  parent& operator=(parent&&) = default;
  virtual ~parent() = default;
};

struct sub_class_1 : public parent {
  int x_{};
};
struct sub_class_2 : public parent {
  int y_{};
};
struct sub_class_3 : public parent {
  int z_{};
};

TEST(match, match) {
  auto obj = sub_class_2{};
  obj.y_ = 77;
  EXPECT_TRUE(utl::match(
                  reinterpret_cast<parent const*>(&obj),
                  [](sub_class_1 const* el) { return el->x_; },
                  [](sub_class_2 const* el) { return el->y_; },
                  [](sub_class_3 const* el) { return el->z_; }) == 77);
}
