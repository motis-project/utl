#pragma once

#include <string>
#include <thread>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "processthreadsapi.h"
#include "windows.h"

namespace utl {

inline void set_thread_name(HANDLE hThread, std::string const& name) {
  auto size_needed =
      MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, nullptr, 0);
  auto wname = std::wstring(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, name.data(), -1, &wname[0], size_needed);
  SetThreadDescription(hThread, wname.c_str());
}

inline void set_thread_name(std::thread& thread, std::string const& name) {
  return set_thread_name(thread.native_handle(), name);
}

inline void set_current_thread_name(std::string const& name) {
  return set_thread_name(GetCurrentThread(), name);
}

}  // namespace utl

#elif defined(__APPLE__)

#include <pthread.h>

namespace utl {

inline void set_thread_name(std::thread&, std::string const&) {}

inline void set_current_thread_name(std::string const& name) {
  pthread_setname_np(name.data());
}

}  // namespace utl

#else

#include <pthread.h>

namespace utl {

inline void set_thread_name(std::thread& thread, std::string const& name) {
  pthread_setname_np(thread.native_handle(), name.data());
}

inline void set_current_thread_name(std::string const& name) {
  pthread_setname_np(pthread_self(), name.data());
}

}  // namespace utl

#endif
