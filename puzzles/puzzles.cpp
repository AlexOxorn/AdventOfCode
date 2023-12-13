#include "puzzles.h"
#include <utility>

yearfunctions pseudo_puzzle_array::operator[](int year){
    switch(year) {
        case 2020: return aoc2020::get_days();
        case 2021: return aoc2021::get_days();
        case 2022: return aoc2022::get_days();
        case 2023: return aoc2023::get_days();
        default: std::unreachable();
    }
}

pseudo_puzzle_array puzzles;
