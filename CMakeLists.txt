cmake_minimum_required(VERSION 3.21)
project(adventofcode2021)

set(CMAKE_CXX_STANDARD 20)

add_executable(adventofcode2021 main.cpp 2021/day01/day01.cpp 2021/day01/day01.h common.h)
