#include "../../../common.h"
#include <algorithm>

namespace aoc2022::day04 {
    struct range {
        int low;
        int high;
    };

    std::istream& operator>>(std::istream& in, std::pair<range, range>& elf_pair) {
        char temp;
        in >> elf_pair.first.low >> temp >> elf_pair.first.high >> temp >> elf_pair.second.low >> temp
                >> elf_pair.second.high;
        return in;
    }

    bool overlap_full(std::pair<range, range> elves) {
        auto& [elf1, elf2] = elves;
        if (elf1.low >= elf2.low && elf1.high <= elf2.high)
            return true;
        if (elf1.low <= elf2.low && elf1.high >= elf2.high)
            return true;
        return false;
    }

    bool overlap(std::pair<range, range> elves) {
        auto& [elf1, elf2] = elves;
        if (elf1.low <= elf2.high /*elf1 is on left*/ && elf1.high >= elf2.low)
            return true;
        if (elf2.low <= elf1.high /*elf1 is on right*/ && elf2.high >= elf1.low)
            return true;
        return false;
    }

    answertype puzzle1(puzzle_options filename) {
        auto groups = get_stream<std::pair<range, range>>(filename);
        size_t count = stdr::distance(groups | stdv::filter(overlap_full));
        myprintf("number of fully overlapping teams is %zu\n", count);
        return count;
    }

    answertype puzzle2(puzzle_options filename) {
        auto groups = get_stream<std::pair<range, range>>(filename);
        size_t count = stdr::distance(groups | stdv::filter(overlap));
        myprintf("number of partially overlapping teams is %zu\n", count);
        return count;
    }
} // namespace aoc2022::day04