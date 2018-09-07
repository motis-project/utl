#pragma once

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <stdexcept>

#ifndef log_err
#define log_err(M, ...) fprintf(stderr, "[ERR] " M "\n", ##__VA_ARGS__);
#endif

#ifndef log_info
#define log_info(M, ...) fprintf(stderr, "[NFO] " M "\n", ##__VA_ARGS__);
#endif

#define LOGGING
#ifdef LOGGING

#ifndef log_time
#define log_time(name, ...)                                          \
  {                                                                  \
    auto start = std::chrono::steady_clock::now();                   \
    { __VA_ARGS__ }                                                  \
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>( \
                  std::chrono::steady_clock::now() - start)          \
                  .count();                                          \
    log_info("%s took %lldms", name, ms);                            \
  }
#endif
#else

#ifndef log_time
#define log_time(name, ...) \
  { __VA_ARGS__ }
#endif

#endif

#ifdef verify
#undef verify
#endif

#define verify(A, M)             \
  if (!(A)) {                    \
    std::cout << M << "\n";      \
    throw std::runtime_error(M); \
  }
