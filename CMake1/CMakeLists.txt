cmake_minimum_required(VERSION 3.10)
project(test)

add_subdirectory ("CMake 1")

add_executable (test ${CMAKE_CURRENT_LIST_DIR}/CMake\ 1/main.cpp)

target_link_libraries (test PRIVATE math M1 M2)
