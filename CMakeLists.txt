cmake_minimum_required(VERSION 3.21)
project(adventofcode2021)

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS)

add_executable(
        adventofcode2021
        main.cpp
        common.h
        2021/day01/day01.cpp
        2021/day01/day01.h
        2021/day02/day02.cpp
        2021/day02/day02.h
        2021/day03/day03.cpp
        2021/day03/day03.h
        2021/day04/day04.cpp
        2021/day04/day04.h
        2021/day05/day05.cpp
        2021/day05/day05.h
        2021/day06/day06.cpp
        2021/day06/day06.h
        2021/day07/day07.cpp
        2021/day07/day07.h
)
