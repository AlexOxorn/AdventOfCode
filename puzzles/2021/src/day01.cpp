#include "../../../common.h"
#include <cstdio>
#include <numeric>
#include "ox/algorithms.h"

#define DAY 01

namespace aoc2021::day01 {
    template<std::input_iterator I>
    int count_difference(I begin, I end, int offset) {
        size_t count = 0;
        ox::predicateCounter<int> counter([](const int& i) {return i>0;}, count);
        ox::offset_difference(begin,end,counter, offset);
        return count;
    }

    template<std::input_iterator I>
    int print_adjacent_increase_count(I begin, I end, int count) {
        auto result = count_difference(begin,end,count);
        myprintf("%d number of increases\n", result);
        return result;
    }

    answertype puzzle1(puzzle_options filename) {
        auto input_vector = get_stream<int>(filename);
        return print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 1);
    }

    answertype puzzle2(puzzle_options filename) {
        auto input_vector = get_stream<int>(filename);
        return print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 3);
    }
}