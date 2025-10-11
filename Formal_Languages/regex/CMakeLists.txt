cmake_minimum_required(VERSION 3.10)
project(RegexModPatternTest CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")
set(COVERAGE_LIBS "-lgcov") 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
add_executable(regex_tests test_regex.cpp regex.h)
target_compile_options(regex_tests PUBLIC "-fprofile-arcs" "-ftest-coverage")
target_link_libraries(regex_tests 
    GTest::gtest_main 
    GTest::gtest 
    "-fprofile-arcs" "-ftest-coverage"
    "-lgcov"
)