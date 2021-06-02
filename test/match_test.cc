#include "catch2/catch.hpp"

#include "utl/match.h"

struct parent {
  virtual ~parent() = default;
};

struct sub_class_1 : public parent {
  int x_;
};
struct sub_class_2 : public parent {
  int y_;
};
struct sub_class_3 : public parent {
  int z_;
};

TEST_CASE("match") {
  auto obj = sub_class_2{};
  obj.y_ = 77;
  CHECK(utl::match(
            reinterpret_cast<parent const*>(&obj),
            [](sub_class_1 const* el) { return el->x_; },
            [](sub_class_2 const* el) { return el->y_; },
            [](sub_class_3 const* el) { return el->z_; }) == 77);
}
