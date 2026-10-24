cmake_minimum_required(VERSION 3.31)
project (ElegantAbstractFactory VERSION 0.0)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-freflection")

enable_testing()

add_subdirectory(inc)
add_subdirectory(tests)
