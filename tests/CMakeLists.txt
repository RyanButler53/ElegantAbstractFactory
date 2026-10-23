find_package(GTest REQUIRED)

macro(EAFTest testname)
    add_executable(${testname} ${testname}.cpp)
    target_link_libraries(${testname} PRIVATE GTest::gtest GTest::gtest_main)
    gtest_discover_tests(${testname})
endmacro()

EAFTest(globalNamespace)
EAFTest(namespacedClasses)
