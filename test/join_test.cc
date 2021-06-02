#include "catch2/catch.hpp"

#include "utl/join.h"

#include <functional>

struct tester {
  tester(std::vector<int> a, std::vector<int> b)
      : a_{std::move(a)}, b_{std::move(b)} {}

  using it_t = std::vector<int>::iterator;

  std::function<void(it_t, it_t, it_t, it_t)> cb_both() {
    return [this](it_t lb_a, it_t ub_a, it_t lb_b, it_t ub_b) {
      ranges_both_.push_back(
          {static_cast<int>(std::distance(std::begin(a_), lb_a)),
           static_cast<int>(std::distance(std::begin(a_), ub_a)),
           static_cast<int>(std::distance(std::begin(b_), lb_b)),
           static_cast<int>(std::distance(std::begin(b_), ub_b))});
    };
  }

  std::function<void(it_t, it_t)> cb_a() {
    return [this](it_t lb_a, it_t ub_a) {
      ranges_a_.push_back(
          {static_cast<int>(std::distance(std::begin(a_), lb_a)),
           static_cast<int>(std::distance(std::begin(a_), ub_a))});
    };
  }

  std::function<void(it_t, it_t)> cb_b() {
    return [this](it_t lb_b, it_t ub_b) {
      ranges_b_.push_back(
          {static_cast<int>(std::distance(std::begin(b_), lb_b)),
           static_cast<int>(std::distance(std::begin(b_), ub_b))});
    };
  }

  bool eq_both(std::vector<std::vector<int>> const& ranges) const {
    return ranges_both_ == ranges;
  }

  bool eq_a(std::vector<std::vector<int>> const& ranges) const {
    return ranges_a_ == ranges;
  }

  bool eq_b(std::vector<std::vector<int>> const& ranges) const {
    return ranges_b_ == ranges;
  }

  std::vector<int> a_;
  std::vector<int> b_;
  std::vector<std::vector<int>> ranges_both_, ranges_a_, ranges_b_;
};

TEST_CASE("inner_join") {
  {
    tester t{{}, {}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({}));
  }

  {
    tester t{{0}, {0}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }
  {
    tester t{{1, 1}, {1, 1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 2}}));
  }
  {
    tester t{{1}, {1, 1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 1, 0, 2}}));
  }
  {
    tester t{{1, 1}, {1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 1}}));
  }

  {
    tester t{{0, 1}, {1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{1, 2, 0, 1}}));
  }
  {
    tester t{{1}, {0, 1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 1, 1, 2}}));
  }
  {
    tester t{{1, 2}, {1}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }
  {
    tester t{{1}, {1, 2}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }

  {
    tester t{{1, 1, 3}, {1, 1, 3}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 2}, {2, 3, 2, 3}}));
  }
  {
    tester t{{1, 1, 2, 3}, {1, 1, 3}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 2}, {3, 4, 2, 3}}));
  }
  {
    tester t{{1, 1, 3}, {1, 1, 2, 3}};
    utl::inner_join(t.a_, t.b_, t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 2}, {2, 3, 3, 4}}));
  }
}

TEST_CASE("inner_join_reverse") {
  {
    tester t{{1, 1}, {1, 1}};
    utl::inner_join(
        t.a_, t.b_, [](auto const& lhs, auto const& rhs) { return lhs > rhs; },
        t.cb_both());
    CHECK(t.eq_both({{0, 2, 0, 2}}));
  }
  {
    tester t{{2, 1}, {2, 1}};
    utl::inner_join(
        t.a_, t.b_, [](auto const& lhs, auto const& rhs) { return lhs > rhs; },
        t.cb_both());
    CHECK(t.eq_both({{0, 1, 0, 1}, {1, 2, 1, 2}}));
  }
}

TEST_CASE("left_join") {
  {
    tester t{{}, {}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{1}, {1}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({}));
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }
  {
    tester t{{0, 1}, {1}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_both({{1, 2, 0, 1}}));
  }
  {
    tester t{{0, 1, 2}, {2}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{0, 1}, {1, 2}}));
    CHECK(t.eq_both({{2, 3, 0, 1}}));
  }
  {
    tester t{{1, 2, 3, 4}, {1}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{1, 2}, {2, 3}, {3, 4}}));
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }
  {
    tester t{{1, 2, 2, 3, 5}, {1, 5}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{1, 3}, {3, 4}}));
    CHECK(t.eq_both({{0, 1, 0, 1}, {4, 5, 1, 2}}));
  }
  {
    tester t{{3}, {2}};
    utl::left_join(t.a_, t.b_, t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_both({}));
  }

  {
    tester t{{3}, {2}};
    utl::left_join(
        t.a_, t.b_, [](auto const& lhs, auto const& rhs) { return lhs > rhs; },
        t.cb_both(), t.cb_a());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_both({}));
  }
}

TEST_CASE("full_join") {
  {
    tester t{{}, {}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({}));
    CHECK(t.eq_b({}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{1}, {1}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({}));
    CHECK(t.eq_b({}));
    CHECK(t.eq_both({{0, 1, 0, 1}}));
  }
  {
    tester t{{2}, {3}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_b({{0, 1}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{1}, {}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_b({}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{}, {1}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({}));
    CHECK(t.eq_b({{0, 1}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{}, {1, 2}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({}));
    CHECK(t.eq_b({{0, 1}, {1, 2}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{}, {1, 2, 2}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({}));
    CHECK(t.eq_b({{0, 1}, {1, 3}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{3}, {1, 2}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_b({{0, 1}, {1, 2}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{3}, {1, 1, 2}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_b({{0, 2}, {2, 3}}));
    CHECK(t.eq_both({}));
  }

  {
    tester t{{2, 4}, {1, 3, 5}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}, {1, 2}}));
    CHECK(t.eq_b({{0, 1}, {1, 2}, {2, 3}}));
    CHECK(t.eq_both({}));
  }
  {
    tester t{{2, 8}, {1, 3, 3, 4, 4, 5}};
    utl::full_join(t.a_, t.b_, t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}, {1, 2}}));
    CHECK(t.eq_b({{0, 1}, {1, 3}, {3, 5}, {5, 6}}));
    CHECK(t.eq_both({}));
  }

  {
    tester t{{2}, {3}};
    utl::full_join(
        t.a_, t.b_, [](auto const& lhs, auto const& rhs) { return lhs > rhs; },
        t.cb_both(), t.cb_a(), t.cb_b());
    CHECK(t.eq_a({{0, 1}}));
    CHECK(t.eq_b({{0, 1}}));
    CHECK(t.eq_both({}));
  }
}
