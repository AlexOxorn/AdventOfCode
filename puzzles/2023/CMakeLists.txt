find_package (Eigen3 3.4 REQUIRED NO_MODULE)

add_library(
        "2023"
        SHARED
        include/2023.h
        src/2023.cpp
        src/day01.cpp
        src/day02.cpp
        src/day03.cpp
        src/day04.cpp
        src/day05.cpp
        src/day06.cpp
        src/day07.cpp
        src/day08.cpp
        src/day09.cpp
        src/day10.cpp
        src/day11.cpp
        src/day12.cpp
        src/day13.cpp
        src/day14.cpp
        src/day15.cpp
        src/day16.cpp
        src/day17.cpp
        src/day18.cpp
        src/day19.cpp
        src/day20.cpp
        src/day21.cpp
        src/day22.cpp
        src/day23.cpp
        src/day24.cpp
        src/day25.cpp
)
target_link_libraries("2023" PRIVATE Eigen3::Eigen)
target_link_libraries("2023" PRIVATE "ox")
target_include_directories("2023" PUBLIC "include")