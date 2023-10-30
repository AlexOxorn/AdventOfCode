//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <numeric>
#include <algorithm>
#include <string>
#include <ox/ranges.h>

#define DAY 03

namespace aoc2022::day03 {
    struct group {
        std::string elves[3];
        friend std::istream& operator>>(std::istream& in, group& g) {
            for (auto& x : g.elves)
                in >> x;
            return in;
        }
    };

    int priorities(char c) {
        if (c >= 'a')
            return c - 'a' + 1;
        return c - 'A' + 27;
    }

    char common_backpack(std::string s) {
        char* start = s.data();
        char* end = s.data() + s.size();
        char* mid = std::midpoint(start, end);

        std::sort(start, mid);
        std::sort(mid, end);

        auto x = stdr::subrange(start, mid) | oxv::set_intersection(stdr::subrange(mid, end));
        return *x.begin();
    }

    char common_badge(group g) {
        for (auto& s : g.elves)
            stdr::sort(s);

        auto intersection = g.elves[0] | oxv::set_intersection(g.elves[1]) | oxv::set_intersection(g.elves[2]);
        return *intersection.begin();
    }

    answertype puzzle1(const char* filename) {
        auto backpack_inputs = get_stream<std::string>(filename);
        auto priorities_stream = backpack_inputs | stdv::transform(common_backpack) | stdv::transform(priorities);
        int priority_sum = std::accumulate(priorities_stream.begin(), priorities_stream.end(), 0);
        myprintf("Total priority sum = %d\n", priority_sum);
        return priority_sum;
    }

    answertype puzzle2(const char* filename) {
        auto group_inputs = get_stream<group>(filename);
        auto priorities_stream = group_inputs | stdv::transform(common_badge) | stdv::transform(priorities);
        int priority_sum = std::accumulate(priorities_stream.begin(), priorities_stream.end(), 0);
        myprintf("Total priority sum = %d\n", priority_sum);
        return priority_sum;
    }
} // namespace aoc2022::day03