#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utl/logging.h"

using ::testing::MatchesRegex;

TEST(log, can_send_info_msg) {
  testing::internal::CaptureStderr();
  utl::log_info("MyCtx", "Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] Message\n"));
};

TEST(log, can_send_debug_msg) {
  testing::internal::CaptureStderr();
  utl::log_debug("MyCtx", "Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[debug\\] \\[logging.+:.+\\] \\[MyCtx\\] Message\n"));
};

TEST(log, can_send_error_msg) {
  testing::internal::CaptureStderr();
  utl::log_error("MyCtx", "Message");
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[error\\] \\[logging.+:.+\\] \\[MyCtx\\] Message\n"));
};

TEST(log, can_format_extra_params) {
  testing::internal::CaptureStderr();
  auto const value = 42;
  utl::log_info("MyCtx", "String={} Int={}", "Hello", value);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] "
                           "String=Hello Int=42\n"));
};

TEST(log, can_have_optional_attrs) {
  testing::internal::CaptureStderr();
  utl::log_info("MyCtx", "Message").attrs({{"key", "value"}});
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] Message\n"));
};
