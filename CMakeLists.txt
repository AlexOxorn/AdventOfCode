cmake_minimum_required(VERSION 3.16)
project(adventofcode)

set(CMAKE_CXX_STANDARD 20)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -fdiagnostics-color=always -fsanitize=address,undefined -g -fconcepts-diagnostics-depth=5")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_CXX_FLAGS "-O2 -fdiagnostics-color=always")
elseif(CMAKE_BUILD_TYPE STREQUAL "Perf")
    SET(CMAKE_CXX_FLAGS "-O0")
endif()
add_subdirectory(ox_lib)

add_executable(
        "${PROJECT_NAME}"
        main.cpp
        common.h
        puzzles/day01/day01.cpp
        puzzles/day02/day02.cpp
        puzzles/day03/day03.cpp
        puzzles/day04/day04.cpp
        puzzles/day05/day05.cpp
        puzzles/day06/day06.cpp
        puzzles/day07/day07.cpp
        puzzles/day08/day08.cpp
        puzzles/day09/day09.cpp
        puzzles/day10/day10.cpp
        puzzles/day11/day11.cpp
        puzzles/day12/day12.cpp
        puzzles/day13/day13.cpp
        puzzles/day14/day14.cpp
        puzzles/day15/day15.cpp
        puzzles/day16/day16.cpp
        puzzles/day17/day17.cpp
        puzzles/day18/day18.cpp
        puzzles/day19/day19.cpp
        puzzles/day20/day20.cpp
        puzzles/day21/day21.cpp
        puzzles/day22/day22.cpp
        puzzles/day23/day23.cpp
        puzzles/day24/day24.cpp
        puzzles/day25/day25.cpp
)

target_link_libraries("${PROJECT_NAME}" PRIVATE "ox")
