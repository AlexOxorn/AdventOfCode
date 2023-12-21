#include "../../../common.h"
#include "ox/math.h"
#include <cassert>
#include <vector>

#define DAY 17

namespace aoc2021::day17 {
    struct target_area : public std::pair<std::pair<int, int>, std::pair<int, int>> {};

    STREAM_IN(target_area, t) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "target area: x=%d..%d, y=%d..%d",
               &t.first.first, &t.first.second, &t.second.second, &t.second.first);
        assert(t.second.second <= 0);
        return in;
    }

    int max_initial_y(target_area t) {
        return -t.second.second - 1;
    }

    int height_from_initial_y(int vy) {
        return ox::triangle_sum(vy);
    }

    bool is_valid_xy(target_area t, int init_x, int init_y) {
        int x = 0, y = 0, vx = init_x, vy = init_y;
        for([[maybe_unused]] int step = 0; ; step++) {
            x += vx;
            y += vy;
            vx = std::max(0, vx - 1);
            vy -= 1;

            if (x > t.first.second)
                return false;
            if (y < t.second.second)
                return false;

            if (x >= t.first.first && y <= t.second.first)
                return true;
        }
    }

    std::vector<std::pair<int, int>> get_all_valid_starts(target_area t) {
        int y = max_initial_y(t);
        std::vector<std::pair<int, int>> to_return;
        for (int init_x : stdv::iota(0, t.first.second+1)) {
            for (int init_y : stdv::iota(-(y + 1), y + 1)) {
                if (is_valid_xy(t, init_x, init_y))
                    to_return.emplace_back(init_x, init_y);
            }
        }
        return to_return;
    }

    answertype puzzle1(puzzle_options filename) {
        target_area t;
        auto input = get_stream<target_area>(filename);
        input >> t;

        int initial_speed = max_initial_y(t);
        int highest = height_from_initial_y(initial_speed);
        myprintf("The highest is %d using initial y speed of %d\n", highest, initial_speed);
        return highest;
    }

    answertype puzzle2(puzzle_options filename) {
        target_area t;
        auto input = get_stream<target_area>(filename);
        input >> t;

        std::vector<std::pair<int, int>> initial_speeds = get_all_valid_starts(t);
        myprintf("The number of valid initial speeds are %zu\n", initial_speeds.size());
        return initial_speeds.size();
    }
} // namespace aoc2021::day17
