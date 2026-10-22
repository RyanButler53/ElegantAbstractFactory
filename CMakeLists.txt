cmake_minimum_required(VERSION 3.31)
project (ElegantAbstractFactory VERSION 0.0)

# hard code compiler since the default compiler doesn't support C++26
set(CMAKE_C_COMPILER "/opt/homebrew/bin/gcc-16")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-16")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-freflection")

enable_testing()
include_directories(inc)
add_subdirectory(tests)
