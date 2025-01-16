#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utl/logging.h"

using ::testing::MatchesRegex;

TEST(log, basic_usage) {
  testing::internal::CaptureStderr();
  utl::info("Simple message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[info\\] \\[logging_test.cc:10\\] Simple message\n"));
};

TEST(log, specifying_ctx) {
  testing::internal::CaptureStderr();
  utl::info("Message").ctx("MyCtx");
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[MyCtx\\] Message\n"));
};

TEST(log, formatting_parameters) {
  testing::internal::CaptureStderr();
  utl::info("String={} Int={}", "Hello", 42);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] String=Hello Int=42\n"));
};
