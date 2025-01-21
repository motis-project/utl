#include <string_view>

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

TEST(log, accept_string_view_as_extra_param) {
  testing::internal::CaptureStderr();
  std::string_view str{"world"};
  utl::log_info("MyCtx", "Hello {}!", str);
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] "
                           "Hello world!\n"));
};

TEST(log, accept_string_view_as_extra_param_inline) {
  testing::internal::CaptureStderr();
  std::string str{"world"};
  utl::log_info("MyCtx", "Hello {}!", std::string_view{str});
  EXPECT_THAT(testing::internal::GetCapturedStderr(),
              MatchesRegex(".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] "
                           "Hello world!\n"));
};

TEST(log, can_have_optional_attrs) {
  testing::internal::CaptureStderr();
  utl::log_info("MyCtx", "Message").attrs({{"key", "value"}});
  EXPECT_THAT(
      testing::internal::GetCapturedStderr(),
      MatchesRegex(
          ".+T.+Z \\[info\\] \\[logging.+:.+\\] \\[MyCtx\\] Message\n"));
};
