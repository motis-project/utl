#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "utl/logging.h"

using ::testing::MatchesRegex;

TEST(log, can_send_info_msg) {
  testing::internal::CaptureStderr();
  utl::info("MyCtx", "Message");
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[utl\\] \\[logging.+:.+\\] "
                           "\\[MyCtx\\] Message\n"));
};

TEST(log, can_send_debug_msg) {
  testing::internal::CaptureStderr();
  utl::debug("MyCtx", "Message");
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[debug\\] \\[utl\\] \\[logging.+:.+\\] "
                           "\\[MyCtx\\] Message\n"));
};

TEST(log, can_send_error_msg) {
  testing::internal::CaptureStderr();
  utl::error("MyCtx", "Message");
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[error\\] \\[utl\\] \\[logging.+:.+\\] "
                           "\\[MyCtx\\] Message\n"));
};

TEST(log, can_format_extra_params) {
  testing::internal::CaptureStderr();
  auto const value = 42;
  utl::info("MyCtx", "String={} Int={}", "Hello", value);
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(".+T.+Z \\[info\\] \\[utl\\] \\[logging.+:.+\\] \\[MyCtx\\] "
                   "String=Hello Int=42\n"));
};

TEST(log, can_have_optional_attrs) {
  testing::internal::CaptureStderr();
  utl::info("MyCtx", "Message").attrs({{"key", "value"}});
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[utl\\] \\[logging.+:.+\\] "
                           "\\[MyCtx\\] Message\n"));
};
