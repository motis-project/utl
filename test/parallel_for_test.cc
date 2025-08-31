#include "gtest/gtest.h"

#include "utl/parallel_for.h"

unsigned long get_random_number() {  // period 2^96-1
  static std::uint64_t x = 123456789, y = 362436069, z = 521288629;

  unsigned long t;
  x ^= x << 16;
  x ^= x >> 5;
  x ^= x << 1;

  t = x;
  x = y;
  y = z;
  z = t ^ x ^ y;

  return z;
}

struct empty {};
TEST(parallel_for, collect) {
  auto next = 0;
  utl::parallel_ordered_collect_threadlocal<empty>(
      100'000,
      [](empty&, std::size_t const i) {
        std::this_thread::sleep_for(
            std::chrono::nanoseconds{get_random_number() % 4096});
        return i;
      },
      [&](std::size_t const i, std::size_t const j) {
        EXPECT_EQ(i, j);
        EXPECT_EQ(j, next++);
      });
  EXPECT_EQ(100'000, next);
}
