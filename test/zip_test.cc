#include "catch/catch.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

#include "utl/to_set.h"
#include "utl/zip.h"

TEST_CASE("zip") {
  SECTION("basic_usage") {
    std::vector<std::string> lhs{"asdf", "bsdf", "csdf"};
    std::vector<int> rhs{1, 2, 3};

    auto const range = utl::zip(lhs, rhs);
    auto it = begin(range);

    {
      REQUIRE(it != end(range));

      auto [str, num] = *it;
      CHECK(str == "asdf");
      CHECK(num == 1);
    }
    {
      ++it;
      REQUIRE(it != end(range));

      auto [str, num] = *it;
      CHECK(str == "bsdf");
      CHECK(num == 2);
    }
    {
      ++it;
      REQUIRE(it != end(range));

      auto [str, num] = *it;
      CHECK(str == "csdf");
      CHECK(num == 3);
    }

    ++it;
    REQUIRE(it == end(range));
  }

  SECTION("safeguard") {
    std::vector<int> vec_short{1, 2, 3};
    std::vector<int> vec_long{1, 2, 3, 4};

    REQUIRE_THROWS(utl::zip(vec_short, vec_long));
  }

  SECTION("self_and_range_for") {
    std::vector<int> vec{1, 2, 3};

    std::vector<int> result;
    for (auto [a, b, c] : utl::zip(vec, vec, vec)) {
      result.push_back(a * b * c);
    }

    REQUIRE(result.size() == 3);
    CHECK(result[0] == 1);
    CHECK(result[1] == 8);
    CHECK(result[2] == 27);
  }

  SECTION("const_input") {
    std::vector<int> lhs{1, 2, 3};
    const std::vector<int> rhs{4, 5, 6};

    for (auto const [a, b] : utl::zip(lhs, rhs)) {
      static_assert(
          !std::is_const<std::remove_reference_t<decltype(a)>>::value);
      static_assert(std::is_const<std::remove_reference_t<decltype(b)>>::value);
    }
  }

  SECTION("force_const_iterator") {
    std::vector<int> vec{1, 2, 3};

    for (auto const [e] : utl::czip(vec)) {
      static_assert(std::is_const<std::remove_reference_t<decltype(e)>>::value);
    }
  }

  SECTION("mutable") {
    std::vector<int> vec{1, 2, 3};
    std::vector<int> vec2{4, 5, 6};

    for (auto const [a, b] : utl::zip(vec, vec2)) {
      a += b;
    }

    REQUIRE(vec.size() == 3);
    CHECK(vec[0] == 5);
    CHECK(vec[1] == 7);
    CHECK(vec[2] == 9);
  }

  SECTION("zero_copy") {
    struct solid_stuff {
      explicit solid_stuff(int val) : val_(val) {}

      solid_stuff(const solid_stuff&) = delete;
      solid_stuff(solid_stuff&&) = delete;
      solid_stuff& operator=(const solid_stuff&) = delete;
      solid_stuff& operator=(solid_stuff&&) = delete;

      int val_;
    };

    std::array<solid_stuff, 3> lhs{
        {solid_stuff{1}, solid_stuff{2}, solid_stuff{3}}};
    std::array<solid_stuff, 3> rhs{
        {solid_stuff{5}, solid_stuff{6}, solid_stuff{7}}};

    std::vector<int> result;
    for (auto const [a, b] : utl::zip(lhs, rhs)) {
      result.push_back(a.val_ + b.val_);
    }

    REQUIRE(result.size() == 3);
    CHECK(result[0] == 6);
    CHECK(result[1] == 8);
    CHECK(result[2] == 10);
  }

  SECTION("interoperability") {
    std::vector<int> vec{1, 2, 3};
    std::vector<int> vec2{3, 2, 1};

    auto const set = utl::to_set(utl::zip(vec, vec2), [](auto const& tup) {
      auto [lhs, rhs] = tup;
      return lhs + rhs;
    });

    REQUIRE(set.size() == 1);
    CHECK(*set.begin() == 4);
  }

  SECTION("more_collections") {
    std::initializer_list<int> lst{1, 2, 3};
    std::map<int, bool> map{{4, true}, {1, false}, {3, true}};
    std::set<std::string> set{"c", "b", "a"};

    std::vector<std::tuple<int, int, bool, std::string>> result;
    for (auto const [i, pair, str] : utl::zip(lst, map, set)) {
      result.emplace_back(i, pair.first, pair.second, str);
    }

    REQUIRE(result.size() == 3);
    CHECK(result[0] == std::make_tuple(1, 1, false, "a"));
    CHECK(result[1] == std::make_tuple(2, 3, true, "b"));
    CHECK(result[2] == std::make_tuple(3, 4, true, "c"));
  }
}
