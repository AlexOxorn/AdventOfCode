add_library(
        "2020"
        include/2020.h
        src/day01.cpp
        src/day02.cpp
        src/day03.cpp)

target_link_libraries("2020" PRIVATE "ox")
target_include_directories("2020" PUBLIC "include")