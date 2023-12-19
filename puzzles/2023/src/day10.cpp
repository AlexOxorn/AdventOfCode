#include "../../../common.h"
#include "pipes.h"
#include <ox/types.h>
#include <unordered_set>

namespace aoc2023::day10 {
    using namespace ox::int_alias;

    PipeDir from_char(char c) {
        switch (c) {
            case 'S': return Start;
            case '|': return V;
            case '-': return H;
            case 'L': return NE;
            case 'J': return NW;
            case '7': return SW;
            case 'F': return SE;
            case '.': return None;
            default: std::unreachable();
        }
    }


    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input_stream = get_stream(filename);
        pipe_structure pipes(input_stream, from_char);
        auto start = pipes.calculate_start();
        long answer = pipes.traverse_path(start);
        pipes.print();
        myprintf("%ld\n", answer);
        return answer;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto input_stream = get_stream(filename);
        pipe_structure pipes(input_stream, from_char);
        auto start = pipes.calculate_start();
        pipes.traverse_path(start);
        long answer = pipes.count_bounded();
        pipes.print();
        myprintf("%ld\n", answer);
        return answer;
    }
} // namespace aoc2023::day10