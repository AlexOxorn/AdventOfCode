//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ranges>
#include <numeric>

namespace aoc2022::day10 {
    enum instruction_type {
        add,
        noop,
    };

    struct instruction {
        instruction_type op;
        int arg;
    };

    std::istream& operator>>(std::istream& in, instruction& inst) {
        std::string s;
        in >> s;
        if (s == "noop")
            inst.op = noop;
        else if (s == "addx") {
            inst.op = add;
            in >> inst.arg;
        }
        return in;
    }

    template <stdr::input_range R, typename Callback>
    void simulate(R& instructions, const Callback& cyclecallback) {
        int X = 1;
        int cycle = 0;
        for (instruction op : instructions) {
            switch (op.op) {
                case noop: cyclecallback(X, cycle); break;
                case add:
                    cyclecallback(X, cycle);
                    cyclecallback(X, cycle);
                    X += op.arg;
            }
        }
    }

    answertype puzzle1(puzzle_options filename) {
        auto instructions = get_stream<instruction>(filename);
        auto checks = stdv::iota(1, 7) | stdv::transform([](int i) { return i * 40 - 20; });
        auto check = checks.begin();
        std::array<long, 6> values{};
        auto out = std::begin(values);

        simulate(instructions, [&] (int& X, int& cycle){
            cycle++;
            if (cycle == *check) {
                *out = X * cycle;
                ++check;
                ++out;
            }
        });

        long sum = std::accumulate(std::begin(values), std::end(values), 0l);
        myprintf("Sum of readings is %ld\n", sum);
        return sum;
    }

    answertype puzzle2(puzzle_options filename) {
        auto instructions = get_stream<instruction>(filename);
        simulate(instructions, [&](int& X, int& cycle) {
            long xpos = cycle%40;
            cycle++;
            myprintf("\033[%dm \033[0m", xpos - 1 <= X && X <= xpos + 1 ? 41 : 0);
            if (xpos == 39)
                myprintf("\n");
        });
        return {};
    }
} // namespace aoc2022::day10