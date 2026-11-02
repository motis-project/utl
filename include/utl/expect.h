#pragma once

#include <cassert>

#include "utl/logging.h"
#include "utl/verify.h"

// Checks an invariant that has a fallback.
// Debug builds: asserts. Release builds: logs an error and returns false.
// The log arguments are only evaluated if the condition does not hold.
#define utl_expect(condition, ctx, ...)                \
  [&]() -> bool {                                      \
    if (!(condition)) {                                \
      UTL_UNLIKELY ::utl::log_error(ctx, __VA_ARGS__); \
      assert(false && #condition);                     \
      return false;                                    \
    }                                                  \
    return true;                                       \
  }()
