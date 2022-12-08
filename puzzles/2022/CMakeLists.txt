add_library(
        "2022"
        include/2022.h
        src/day01.cpp
        src/day02.cpp
        src/day03.cpp
        src/day04.cpp
        src/day05.cpp
        src/day06.cpp
)

target_link_libraries("2022" PRIVATE "ox")
target_include_directories("2022" PUBLIC "include")