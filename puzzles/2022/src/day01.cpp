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

        auto operator<=>(const elf& other) const { return calories <=> other.calories; }
        bool operator==(const elf& other) const = default;
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

    void puzzle1(const char* filename) {
        auto elves = get_stream<elf>(filename);
        auto max = stdr::max(elves);
        printf("the max elf is %ld\n", max.calories);
    }

    void puzzle2(const char* filename) {
        auto elf_stream = get_stream<elf>(filename);
        std::vector elves(elf_stream.begin(), elf_stream.end());
        stdr::partial_sort(elves, elves.begin() + 3, std::greater<>());
        auto total = std::accumulate(
                elves.begin(), elves.begin() + 3, elf{});
        printf("the max three elves sum to %ld", total.calories);
    }
} // namespace aoc2022::day01