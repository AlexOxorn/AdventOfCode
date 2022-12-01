add_library(
        "2022"
        include/2022.h
        src/day01.cpp
)

target_link_libraries("2022" PRIVATE "ox")
target_include_directories("2022" PUBLIC "include")