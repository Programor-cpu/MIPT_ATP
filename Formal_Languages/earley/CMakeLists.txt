set(CMAKE_CXX_COMPILER g++-13)
set(CMAKE_C_COMPILER gcc-13)

cmake_minimum_required(VERSION 3.10)
project(L1CFL_Parser CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(GTest REQUIRED)
include(GoogleTest)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")


add_executable(cfl_tests cfl_unit_tests.cpp earley.h)


target_link_libraries(cfl_tests GTest::gtest_main)


gtest_discover_tests(cfl_tests)