project(utl)
cmake_minimum_required(VERSION 3.0)

include_directories(include)

file(GLOB_RECURSE utl-test-files test/*.cc)

if (MSVC)
  file(GLOB_RECURSE zip-test test/zip_test.cc)
  list(REMOVE_ITEM utl-test-files ${zip-test})
endif()

add_executable(utl-test ${utl-test-files})
if (MSVC)
  set_target_properties(utl-test PROPERTIES COMPILE_FLAGS "/std:c++latest")
else()
  set_target_properties(utl-test PROPERTIES COMPILE_FLAGS "-std=c++1z -Wall -Wextra")
endif()