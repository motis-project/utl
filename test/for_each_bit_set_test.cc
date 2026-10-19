#include "gtest/gtest.h"

#include <cstdint>
#include <limits>
#include <random>
#include <vector>

#include "utl/for_each_bit_set.h"

namespace {

template <typename T>
std::vector<unsigned> bits_of(T const v) {
  auto b = std::vector<unsigned>{};
  utl::for_each_set_bit(v, [&](auto const bit) { b.push_back(bit); });
  return b;
}

// The straightforward scan over every bit position, which is what the
// population count driven implementation has to agree with.
template <typename T>
std::vector<unsigned> reference_bits_of(T const v) {
  auto b = std::vector<unsigned>{};
  for (auto bit = 0U; bit != sizeof(T) * 8U; ++bit) {
    if (utl::has_bit_set(v, bit)) {
      b.push_back(bit);
    }
  }
  return b;
}

template <typename T>
void expect_agrees_with_reference() {
  auto rng = std::mt19937_64{12345U};
  for (auto i = 0; i != 10000; ++i) {
    auto const v = static_cast<T>(rng());
    EXPECT_EQ(reference_bits_of(v), bits_of(v)) << "value " << +v;
  }

  constexpr auto const width = static_cast<unsigned>(sizeof(T) * 8U);
  for (auto const v : {T{0}, static_cast<T>(~T{0}), T{1},
                       static_cast<T>(T{1} << (width - 1U))}) {
    EXPECT_EQ(reference_bits_of(v), bits_of(v)) << "value " << +v;
  }
}

}  // namespace

TEST(for_each_bit_set, empty) {
  EXPECT_TRUE(bits_of(std::uint64_t{0U}).empty());
}

TEST(for_each_bit_set, ascending_order) {
  EXPECT_EQ((std::vector<unsigned>{0U, 3U, 7U}),
            bits_of(std::uint8_t{0b1000'1001U}));
}

TEST(for_each_bit_set, highest_bit) {
  EXPECT_EQ((std::vector<unsigned>{63U}), bits_of(std::uint64_t{1U} << 63U));
}

TEST(for_each_bit_set, all_bits) {
  EXPECT_EQ(64U, bits_of(~std::uint64_t{0U}).size());
}

TEST(for_each_bit_set, agrees_with_reference) {
  expect_agrees_with_reference<std::uint8_t>();
  expect_agrees_with_reference<std::uint16_t>();
  expect_agrees_with_reference<std::uint32_t>();
  expect_agrees_with_reference<std::uint64_t>();
}

TEST(for_each_bit_set, signed_negative) {
  // `levels` arriving as a negative signed value used to shift into the sign
  // bit; the unsigned conversion visits every bit instead.
  EXPECT_EQ(32U, bits_of(std::int32_t{-1}).size());
  EXPECT_EQ((std::vector<unsigned>{31U}),
            bits_of(std::numeric_limits<std::int32_t>::min()));
}

TEST(for_each_bit_set, constexpr_usable) {
  constexpr auto const n = [] {
    auto c = 0U;
    utl::for_each_set_bit(std::uint32_t{0b1011U}, [&](auto const) { ++c; });
    return c;
  }();
  static_assert(n == 3U);
  EXPECT_EQ(3U, n);
}
