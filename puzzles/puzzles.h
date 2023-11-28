//
// Created by alexoxorn on 10/8/22.
//

#ifndef ADVENTOFCODE_PUZZLES_H
#define ADVENTOFCODE_PUZZLES_H

#include <array>
#include "../common.h"
#include <2020.h>
#include <2021.h>
#include <2022.h>
#include <2023.h>

constexpr std::array<yearfunctions, 4> puzzles {{
         aoc2020::days,
         aoc2021::days,
         aoc2022::days,
         aoc2023::days,
}};

#endif // ADVENTOFCODE_PUZZLES_H
