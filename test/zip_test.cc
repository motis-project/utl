#include "gtest/gtest.h"

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "utl/to_set.h"
#include "utl/zip.h"

TEST(zip, basic_usage) {
  std::vector<std::string> lhs{"asdf", "bsdf", "csdf"};
  std::vector<int> rhs{1, 2, 3};

  auto const range = utl::zip(lhs, rhs);
  auto it = begin(range);

  {
    ASSERT_TRUE(it != end(range));

    auto [str, num] = *it;
    EXPECT_TRUE(str == "asdf");
    EXPECT_TRUE(num == 1);
  }
  {
    ++it;
    ASSERT_TRUE(it != end(range));

    auto [str, num] = *it;
    EXPECT_TRUE(str == "bsdf");
    EXPECT_TRUE(num == 2);
  }
  {
    ++it;
    ASSERT_TRUE(it != end(range));

    auto [str, num] = *it;
    EXPECT_TRUE(str == "csdf");
    EXPECT_TRUE(num == 3);
  }

  ++it;
  ASSERT_TRUE(it == end(range));
}

TEST(zip, safeguard) {
  std::vector<int> vec_short{1, 2, 3};
  std::vector<int> vec_long{1, 2, 3, 4};

  ASSERT_ANY_THROW(utl::zip(vec_short, vec_long));
}

TEST(zip, self_and_range_for) {
  std::vector<int> vec{1, 2, 3};

  std::vector<int> result;
  for (auto [a, b, c] : utl::zip(vec, vec, vec)) {
    result.push_back(a * b * c);
  }

  ASSERT_TRUE(result.size() == 3);
  EXPECT_TRUE(result[0] == 1);
  EXPECT_TRUE(result[1] == 8);
  EXPECT_TRUE(result[2] == 27);
}

TEST(zip, move_types_vs_ref_types) {
  auto ref = std::vector<int>{};
  static_assert(std::is_lvalue_reference_v<decltype(std::get<0>(
                    utl::zip(ref, std::vector<int>{}).tup_))>);
  static_assert(std::is_rvalue_reference_v<decltype(std::get<1>(
                    utl::zip(ref, std::vector<int>{}).tup_))>);
}

TEST(zip, const_input) {
  std::vector<int> lhs{1, 2, 3};
  const std::vector<int> rhs{4, 5, 6};

  static_assert(!std::is_const<std::remove_reference_t<decltype(std::get<0>(
                    *utl::zip(lhs, rhs).begin()))>>::value);
  static_assert(std::is_const<std::remove_reference_t<decltype(std::get<1>(
                    *utl::zip(lhs, rhs).begin()))>>::value);
}

TEST(zip, force_const_iterator) {
  std::vector<int> vec{1, 2, 3};
  static_assert(std::is_const<std::remove_reference_t<decltype(std::get<0>(
                    *utl::czip(vec).begin()))>>::value);
}

TEST(zip, mutable) {
  std::vector<int> vec{1, 2, 3};
  std::vector<int> vec2{4, 5, 6};

  for (auto const [a, b] : utl::zip(vec, vec2)) {
    a += b;
  }

  ASSERT_TRUE(vec.size() == 3);
  EXPECT_TRUE(vec[0] == 5);
  EXPECT_TRUE(vec[1] == 7);
  EXPECT_TRUE(vec[2] == 9);
}

TEST(zip, zero_copy) {
  struct solid_stuff {
    explicit solid_stuff(int val) : val_(val) {}

    solid_stuff(const solid_stuff&) = delete;
    solid_stuff(solid_stuff&&) = delete;
    solid_stuff& operator=(const solid_stuff&) = delete;
    solid_stuff& operator=(solid_stuff&&) = delete;
    ~solid_stuff() = default;

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

  ASSERT_TRUE(result.size() == 3);
  EXPECT_TRUE(result[0] == 6);
  EXPECT_TRUE(result[1] == 8);
  EXPECT_TRUE(result[2] == 10);
}

TEST(zip, interoperability) {
  std::vector<int> vec{1, 2, 3};
  std::vector<int> vec2{3, 2, 1};

  auto const set = utl::to_set(utl::zip(vec, vec2), [](auto const& tup) {
    auto [lhs, rhs] = tup;
    return lhs + rhs;
  });

  ASSERT_TRUE(set.size() == 1);
  EXPECT_TRUE(*set.begin() == 4);
}

TEST(zip, more_collections) {
  std::initializer_list<int> lst{1, 2, 3};
  std::map<int, bool> map{{4, true}, {1, false}, {3, true}};
  std::set<std::string> set{"c", "b", "a"};

  std::vector<std::tuple<int, int, bool, std::string>> result;
  for (auto const [i, pair, str] : utl::zip(lst, map, set)) {
    result.emplace_back(i, pair.first, pair.second, str);
  }

  ASSERT_TRUE(result.size() == 3);
  EXPECT_TRUE(result[0] == std::make_tuple(1, 1, false, "a"));
  EXPECT_TRUE(result[1] == std::make_tuple(2, 3, true, "b"));
  EXPECT_TRUE(result[2] == std::make_tuple(3, 4, true, "c"));
}

TEST(zip, algorithm) {
  std::vector<int> vec{1, 2, 3};
  std::vector<int> vec2{2, 2, 1};

  EXPECT_TRUE(std::all_of(
      begin(utl::zip(vec, vec2)), end(utl::zip(vec, vec2)),
      [](auto const& t) { return std::get<0>(t) > 0 && std::get<1>(t) > 0; }));
  EXPECT_TRUE(!std::any_of(begin(utl::zip(vec, vec2)), end(utl::zip(vec, vec2)),
                           [](auto const& t) {
                             return std::get<0>(t) <= 0 || std::get<1>(t) <= 0;
                           }));
}
