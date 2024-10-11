#include "gtest/gtest.h"

#include "utl/init_from.h"

struct a {
  int& i_;
  float& j_;
  int& k_;
};

struct b {
  double z_;
  std::unique_ptr<float> x_;
  int y_;
};

template <typename T>
concept C3 = requires(T t) {
  T{utl::detail::wildcard{}, utl::detail::wildcard{}, utl::detail::wildcard{}};
};

static_assert(C3<a>);
static_assert(C3<b>);

TEST(init_from, init_from) {
  struct test {};

  struct a {
    int& i_;
    float& j_;
    int& k_;
    test* str_;
  };

  struct b {
    double z_;
    std::unique_ptr<float> x_;
    int y_;
    std::unique_ptr<test> str_;
  };

  static_assert(utl::detail::is_brace_constructible<b, 3U>());
  static_assert(utl::detail::arity<a>() == 4U);
  static_assert(utl::detail::arity<b>() == 4U);

  auto src = b{0.0, std::make_unique<float>(10.0F), 42, nullptr};
  auto tgt = utl::init_from<a>(src);
  ASSERT_TRUE(tgt.has_value());

  EXPECT_EQ(42, tgt->i_);
  EXPECT_EQ(10.0F, tgt->j_);
  EXPECT_EQ(42, tgt->k_);
  EXPECT_EQ(nullptr, tgt->str_);

  tgt->i_ = 7.0F;
  tgt->j_ = 77.0F;
  EXPECT_EQ(77.0F, *src.x_);
  EXPECT_EQ(7.0F, src.y_);

  tgt->k_ = 99;
  EXPECT_EQ(99, src.y_);

  auto no = utl::init_from<a>(b{});
  EXPECT_FALSE(no.has_value());

  src.str_ = std::make_unique<test>();
  auto tgt1 = utl::init_from<a>(src);
  EXPECT_EQ(tgt1->str_, src.str_.get());
};
