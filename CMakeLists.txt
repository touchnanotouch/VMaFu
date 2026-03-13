cmake_minimum_required(VERSION 3.27)
project(vmafu VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# MPI option (passed from Makefile)

option(VMAFU_USE_MPI "Enable MPI support" OFF)

# Main library

add_library(vmafu INTERFACE)
target_include_directories(vmafu INTERFACE ${CMAKE_SOURCE_DIR}/include)

# Add MPI if enabled

if(VMAFU_USE_MPI)
    message(STATUS "MPI support: ENABLED")
    find_package(MPI REQUIRED)
    target_link_libraries(vmafu INTERFACE MPI::MPI_CXX)
    target_compile_definitions(vmafu INTERFACE VMAFU_USE_MPI)
else()
    message(STATUS "MPI support: DISABLED")
endif()

# Demo executable

add_executable(demo demo/main.cpp)
target_link_libraries(demo vmafu)
