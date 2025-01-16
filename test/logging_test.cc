#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utl/logging.h"

using ::testing::MatchesRegex;

TEST(log, can_send_info_msg) {
  testing::internal::CaptureStderr();
  utl::info("Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(".+T.+Z \\[info\\] \\[logging_test.cc:..\\] Message\n"));
};

TEST(log, can_send_debug_msg) {
  testing::internal::CaptureStderr();
  utl::debug("Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(".+T.+Z \\[debug\\] \\[logging_test.cc:..\\] Message\n"));
};

TEST(log, can_send_error_msg) {
  testing::internal::CaptureStderr();
  utl::error("Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(".+T.+Z \\[error\\] \\[logging_test.cc:..\\] Message\n"));
};

TEST(log, can_format_extra_params) {
  testing::internal::CaptureStderr();
  auto const value = 42;
  utl::info("String={} Int={}", "Hello", value);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[logging_test.cc:..\\] "
                           "String=Hello Int=42\n"));
};

TEST(log, can_have_an_optional_ctx) {
  testing::internal::CaptureStderr();
  utl::info("Message").ctx("MyCtx");
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[logging_test.cc:..\\] "
                           "\\[MyCtx\\] Message\n"));
};

TEST(log, can_have_optional_metadata) {
  testing::internal::CaptureStderr();
  utl::info("Message").metadata({{"key", "value"}});
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(".+T.+Z \\[info\\] \\[logging_test.cc:..\\] Message\n"));
};
