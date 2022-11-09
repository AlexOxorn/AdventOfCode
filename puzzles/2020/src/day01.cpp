//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <cstdio>
#include <numeric>
#include <unordered_set>
#include "ox/algorithms.h"

#define DAY 01

namespace aoc2020::day01 {
    std::pair<int, int> findsum(const std::unordered_set<int>& set, int target_sum) {
        for (int i : set) {
            if (set.contains(target_sum - i))
                return {i, target_sum - i};
        }
        return {0, target_sum};
    }

    std::array<int, 3> find_triple_sum(const std::unordered_set<int>& set, int target_sum) {
        for (int i : set) {
            auto [a, b] = findsum(set, target_sum - i);
            if (a)
                return {i, a, b};
        }
        return {0, 0, target_sum};
    }

    void puzzle1(const char* filename) {
        auto input_vector = get_stream<int>(filename);
        std::unordered_set expenses(input_vector.begin(), input_vector.end());
        auto [exp1, exp2] = findsum(expenses, 2020);
        printf("expense 1 was %d\nexpense 2 was %d\ntheir product is %d\n\n", exp1, exp2, exp1 * exp2);
    }

    void puzzle2(const char* filename) {
        auto input_vector = get_stream<int>(filename);
        std::unordered_set expenses(input_vector.begin(), input_vector.end());
        auto [a, b, c] = find_triple_sum(expenses, 2020);
        printf("expense 1 was %d\nexpense 2 was %d\nexpense 3 was %d\ntheir product is %d", a, b, c, a * b * c);
    }
}