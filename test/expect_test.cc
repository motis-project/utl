#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utl/expect.h"

using ::testing::MatchesRegex;

TEST(expect, holds) {
  auto n_evaluated = 0U;
  auto const count = [&]() { return ++n_evaluated; };
  EXPECT_TRUE(utl_expect(1 + 1 == 2, "MyCtx", "Message {}", count()));
  EXPECT_EQ(0U, n_evaluated);
}

#ifdef NDEBUG
TEST(expect, fails_logs_error) {
  testing::internal::CaptureStderr();
  EXPECT_FALSE(utl_expect(1 + 1 == 3, "MyCtx", "Message {}", 42));
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[error\\] \\[expect.+:..\\] \\[MyCtx\\] "
                           "Message 42\n"));
}
#else
TEST(expect, fails_asserts) {
  EXPECT_DEATH(utl_expect(1 + 1 == 3, "MyCtx", "Message {}", 42), "");
}
#endif
