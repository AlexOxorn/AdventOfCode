cmake_minimum_required(VERSION 3.16)
project(adventofcode)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -fdiagnostics-color=always -fsanitize=address,undefined -g -fconcepts-diagnostics-depth=5")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_CXX_FLAGS "-O2 -fdiagnostics-color=always")
endif()
add_subdirectory(ox_lib)

add_executable(
        "${PROJECT_NAME}"
        main.cpp
        common.h
        2021/day01/day01.cpp
        2021/day02/day02.cpp
        2021/day03/day03.cpp
        2021/day04/day04.cpp
        2021/day05/day05.cpp
        2021/day06/day06.cpp
        2021/day07/day07.cpp
        2021/day08/day08.cpp
        2021/day09/day09.cpp
        2021/day10/day10.cpp
        2021/day11/day11.cpp
        2021/day12/day12.cpp
        2021/day13/day13.cpp
        2021/day14/day14.cpp
        2021/day15/day15.cpp
        2021/day16/day16.cpp
        2021/day17/day17.cpp
        2021/day18/day18.cpp
)

target_link_libraries("${PROJECT_NAME}" PRIVATE "ox")
