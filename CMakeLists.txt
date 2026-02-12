cmake_minimum_required(VERSION 3.27)
project(vmafu VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# MPI option (passed from Makefile)

option(USE_MPI "Enable MPI support" OFF)

# Main library

add_library(vmafu INTERFACE)
target_include_directories(vmafu INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Add MPI if enabled

if(USE_MPI)
    message(STATUS "MPI support: ENABLED")
    find_package(MPI REQUIRED)
    target_link_libraries(vmafu INTERFACE MPI::MPI_CXX)
    target_compile_definitions(vmafu INTERFACE USE_MPI)
else()
    message(STATUS "MPI support: DISABLED")
endif()

# Tests

enable_testing()
add_library(test-utils INTERFACE)
target_include_directories(test-utils INTERFACE ${CMAKE_SOURCE_DIR}/tests)

# Demo executable

add_executable(demo demo/main.cpp)
target_link_libraries(demo vmafu)

# Test suite

add_executable(test-suite tests/test-suite.cpp)
target_link_libraries(test-suite vmafu test-utils)
add_test(NAME test-suite COMMAND test-suite)
