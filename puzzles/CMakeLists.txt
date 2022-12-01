add_subdirectory("2020")
add_subdirectory("2021")
add_subdirectory("2022")

add_library("puzzles" "puzzles.h")
target_include_directories(puzzles PUBLIC .)
target_link_libraries(puzzles 2022)
target_link_libraries(puzzles 2021)
target_link_libraries(puzzles 2020)
