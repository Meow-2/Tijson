cmake_minimum_required(VERSION 3.15)

project(Tijson VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)

add_executable(test test/test_parse_literal.cc test/test_parse_number.cc test/tijson.cc)
target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(GTest_FOUND)
    target_link_libraries(test GTest::gtest GTest::gtest_main)
else(GTest_FOUND)
    message(FATAL_ERROR ”GTest library not found”)
endif(GTest_FOUND)
