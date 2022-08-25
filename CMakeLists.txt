cmake_minimum_required(VERSION 3.15)

project(Tijson VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(PROJECT_BINARY_DIR STREQUAL PROJECT_SOURCE_DIR)
  message(
    WARNING
      "The binary directory of CMake cannot be the same as source directory!")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB_RECURSE TEST_DIR_LIST "test/*.cc")

add_executable(test ${TEST_DIR_LIST})

target_include_directories(test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(GTest REQUIRED)

if(GTest_FOUND)
  target_link_libraries(test GTest::gtest GTest::gtest_main)
else(GTest_FOUND)
  message(FATAL_ERROR ”GTest library not found”)
endif(GTest_FOUND)

if(NOT MSVC)
  find_program(CCACHE_PROGRAM ccache)
  if(CCACHE_PROGRAM)
    message(STATUS "Found CCache: ${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_PROGRAM})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CCACHE_PROGRAM})
  endif()
endif()
