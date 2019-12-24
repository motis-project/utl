project(utl)
cmake_minimum_required(VERSION 3.16)

find_package(Boost)

include(cmake/pkg.cmake)

add_library(utl INTERFACE)
target_link_libraries(utl INTERFACE fmt-header-only)
target_include_directories(utl INTERFACE include)
target_include_directories(utl SYSTEM INTERFACE ${Boost_INCLUDE_DIR})
target_compile_options(utl INTERFACE -D_SCL_SECURE_NO_WARNINGS=1)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(utl-compile-flags "\
     -Wall \
     -Wextra \
     -Werror \
     -Wno-gnu-zero-variadic-macro-arguments \
     -std=c++17 \
  ")
elseif(NOT MSVC)
  set(utl-compile-flags "\
     -Wall \
     -Wextra \
     -Werror \
     -std=c++17 \
     -Wno-unused-but-set-parameter \
  ")
else()
  set(utl-compile-flags "\
     /std:c++latest
  ")
endif()

file(GLOB_RECURSE utl-test-files test/*.cc)
if (MSVC)
  file(GLOB_RECURSE zip-test test/zip_test.cc)
  list(REMOVE_ITEM utl-test-files ${zip-test})
endif()
add_executable(utl-test ${utl-test-files})
target_link_libraries(utl-test utl)
set_target_properties(utl-test PROPERTIES COMPILE_FLAGS ${utl-compile-flags})

add_custom_target(utl-format-check
    find
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/test
      -type f
      -not -name "catch.hpp"
      -a
      \(
      -name "*.cc"
      -o -name "*.h"
      \)
      -print0
    | xargs -0 clang-format-5.0 -i
  COMMAND
    git status --porcelain | xargs -I {} -0 test -z \"{}\"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Checking source code formatting"
  VERBATIM
)

add_custom_target(utl-lint
  /usr/bin/python
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/run-clang-tidy.py
      -exit-on-error
      -header-filter "^${CMAKE_CURRENT_SOURCE_DIR}/include"
      "^${CMAKE_CURRENT_SOURCE_DIR}/test/"
  COMMENT "Running clang-tidy"
  VERBATIM
)
add_dependencies(utl-lint utl-test)