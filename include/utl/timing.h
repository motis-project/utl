#pragma once

#include <chrono>

#define UTL_START_TIMING(_X) auto _X##_start = std::chrono::steady_clock::now()
#define UTL_STOP_TIMING(_X) auto _X##_stop = std::chrono::steady_clock::now()
#define UTL_TIMING_MS(_X)                                            \
  (std::chrono::duration_cast<std::chrono::milliseconds>(_X##_stop - \
                                                         _X##_start) \
       .count())
#define UTL_TIMING_US(_X)                                            \
  (std::chrono::duration_cast<std::chrono::microseconds>(_X##_stop - \
                                                         _X##_start) \
       .count())

#define UTL_GET_TIMING_MS(_X)                             \
  (std::chrono::duration_cast<std::chrono::milliseconds>( \
       std::chrono::steady_clock::now() - _X##_start)     \
       .count())

#define UTL_GET_TIMING_US(_X)                             \
  (std::chrono::duration_cast<std::chrono::microseconds>( \
       std::chrono::steady_clock::now() - _X##_start)     \
       .count())
