#include "../../../common.h"
#include <unordered_set>

namespace aoc2022::day06 {
    template <std::random_access_iterator I>
    I first_mismatch(I begin, I window, I end) {
        for (; window != end; ++window, ++begin) {
            std::unordered_set chunk(begin, window);
            if (static_cast<long>(chunk.size()) == (window - begin)) {
                return window;
            }
        }
        return window;
    }

    answertype solve(puzzle_options filename, int windowsize) {
        std::vector<char> signal = get_from_input<char>(filename);
        std::random_access_iterator auto position = first_mismatch(signal.begin(), signal.begin() + windowsize, signal.end());
        char output_message[44];
        sprintf(output_message, "The signal start is %%.%.*ds at position %%zu\n", (windowsize >= 10) + 1, windowsize);
        myprintf(output_message, position.base() - windowsize, position - signal.begin());
        return position - signal.begin();
    }

    answertype puzzle1(puzzle_options filename) {
        return solve(filename, 4);
    }

    answertype puzzle2(puzzle_options filename) {
        return solve(filename, 14);
    }
} // namespace aoc2022::day05