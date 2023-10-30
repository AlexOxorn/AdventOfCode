//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ox/math.h>

#define DAY 02

namespace aoc2022::day02 {
    struct round {
        int me, opponent;

        friend std::istream& operator>>(std::istream& in, round& r);

        [[nodiscard]] auto round_result() const {
            int res = ox::mod(me - opponent + 1, 3);
            return res * 3 + me + 1;
        }

        [[nodiscard]] int round_result2() const {
            int res = me;
            int me = ox::mod(opponent + res - 1, 3);
            return res * 3 + me + 1;
        }
    };

    std::istream& operator>>(std::istream& in, round& r) {
        unsigned char played, play;
        in >> played >> play;
        r.me = play - 'X';
        r.opponent = played - 'A';
        return in;
    }

    answertype puzzle1(const char* filename) {
        auto inputstream = get_stream<round>(filename);
        auto x = inputstream | stdv::transform(&round::round_result);
        int total_score = std::accumulate(x.begin(), x.end(), 0);
        myprintf("Total Score   = %d\n", total_score);
        return total_score;
    }

    answertype puzzle2(const char* filename) {
        auto inputstream = get_stream<round>(filename);
        auto x = inputstream | stdv::transform(&round::round_result2);
        int total_score = std::accumulate(x.begin(), x.end(), 0);
        myprintf("Total Score 2 = %d\n", total_score);
        return total_score;
    }
} // namespace aoc2022::day02