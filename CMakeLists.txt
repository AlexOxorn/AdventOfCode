cmake_minimum_required(VERSION 3.16)
project(adventofcode)

set(CMAKE_CXX_STANDARD 23)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -fdiagnostics-color=always -fsanitize=undefined,address -g")
#    SET(CMAKE_CXX_FLAGS "-O0 -Wall -Wextra -fdiagnostics-color=always -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    SET(CMAKE_CXX_FLAGS "-O3 -fdiagnostics-color=always -march=native")
elseif(CMAKE_BUILD_TYPE STREQUAL "Perf")
    SET(CMAKE_CXX_FLAGS "-Og")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fexperimental-library")
endif ()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts-diagnostics-depth=2")

set(OXLIB_CANVAS ON)

add_subdirectory(ox_lib)
add_subdirectory(puzzles)

add_executable(
        "${PROJECT_NAME}"
        main.cpp
        common.h
        tester.cpp
        tester.h
)

find_package(fmt REQUIRED)

target_link_libraries("${PROJECT_NAME}" PRIVATE "ox")
target_link_libraries("${PROJECT_NAME}" PRIVATE "puzzles")
