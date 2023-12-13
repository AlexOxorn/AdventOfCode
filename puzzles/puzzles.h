#ifndef ADVENTOFCODE_PUZZLES_H
#define ADVENTOFCODE_PUZZLES_H

#include <2020.h>
#include <2021.h>
#include <2022.h>
#include <2023.h>

struct pseudo_puzzle_array {
    yearfunctions operator[](int year);
    static int current_year() { return 2023; }
};

extern pseudo_puzzle_array puzzles;

#endif // ADVENTOFCODE_PUZZLES_H
