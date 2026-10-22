# include(FetchContent)

# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 
# )
# FetchContent_MakeAvailable(googletest)

# include(GoogleTest)

add_executable(validTest test1.cpp)
# target_link_libraries(validTest gtest gtest_main)
