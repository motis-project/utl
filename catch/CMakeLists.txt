project(catch)

add_library(catch INTERFACE)
target_include_directories(catch SYSTEM INTERFACE include)
target_compile_features(catch INTERFACE cxx_std_14)