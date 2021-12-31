//
// Created by alexoxorn on 2021-12-01.
//

#include "day01.h"
#include <cstdio>
#include <numeric>
#include <ox/algorithms.h>

#define YEAR 2021
#define DAY 01

namespace day01 {
    template<std::input_iterator I>
    int count_difference(I begin, I end, int offset) {
        size_t count = 0;
        ox::predicateCounter<int> counter([](const int& i) {return i>0;}, count);
        ox::offset_difference(begin,end,counter, offset);
        return count;
    }

    template<std::input_iterator I>
    void print_adjacent_increase_count(I begin, I end, int count) {
        std::printf("%d number of increases\n", count_difference(begin,end,count));
    }

    void puzzle1() {
        auto input_vector = GET_STREAM(input, int);
        print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 1);
    }

    void puzzle2() {
        auto input_vector = GET_STREAM(input, int);
        print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 3);
    }
}