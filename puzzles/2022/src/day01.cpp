//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <algorithm>
#include <numeric>

#define DAY 01

namespace aoc2022::day01 {
    struct elf {
        long calories;

        friend std::istream& operator>>(std::istream& in, elf& self);

        elf operator+(const elf& other) const {
            return elf{calories + other.calories};
        }
    };

    std::istream& operator>>(std::istream& in, elf& self) {
        std::string line;
        self.calories = 0;
        while (std::getline(in, line) && !line.empty()) {
            self.calories += std::stoi(line);
        }
        return in;
    }

    answertype puzzle1(puzzle_options filename) {
        auto elves = get_stream<elf>(filename);
        auto max = stdr::max(elves, std::less<>(), &elf::calories);
        myprintf("the max elf is %ld\n", max.calories);
        return max.calories;
    }

    answertype puzzle2(puzzle_options filename) {
        auto elf_stream = get_stream<elf>(filename);
        std::vector elves(elf_stream.begin(), elf_stream.end());
        stdr::nth_element(elves, elves.begin() + 3, std::greater<>(), &elf::calories);
        auto total = std::accumulate(elves.begin(), elves.begin() + 3, elf{});
        myprintf("the max three elves sum to %ld", total.calories);
        return total.calories;
    }
} // namespace aoc2022::day01