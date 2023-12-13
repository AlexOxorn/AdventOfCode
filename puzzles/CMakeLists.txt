add_subdirectory("2020")
add_subdirectory("2021")
add_subdirectory("2022")
add_subdirectory("2023")

add_library("puzzles" "puzzles.h" "puzzles.cpp")
set_target_properties("puzzles" PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(puzzles PUBLIC .)

target_link_libraries(puzzles ox)
target_link_libraries(puzzles 2023)
target_link_libraries(puzzles 2022)
target_link_libraries(puzzles 2021)
target_link_libraries(puzzles 2020)
