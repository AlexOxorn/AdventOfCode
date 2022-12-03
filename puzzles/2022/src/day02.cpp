//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <algorithm>
#include <numeric>
#include <ox/math.h>

#define DAY 02

namespace aoc2022::day02 {
    struct round {
        int me, opponent;

        friend std::istream& operator>>(std::istream& in, round& r);

        [[nodiscard]] auto round_result() const {
            ox::modulo<3, int> res = me - opponent + 1;
            return res.result * 3 + me + 1;
        }

        [[nodiscard]] int round_result2() const {
            int res = me;
            ox::modulo<3, int> me = opponent + res - 1;
            return res * 3 + me.result + 1;
        }
    };

    std::istream& operator>>(std::istream& in, round& r) {
        unsigned char played, play;
        in >> played >> play;
        r.me = play - 'X';
        r.opponent = played - 'A';
        return in;
    }

    void puzzle1(const char* filename) {
        auto inputstream = get_stream<round>(filename);
        auto x = inputstream | stdv::transform(&round::round_result);
        int total_score = std::accumulate(x.begin(), x.end(), 0);
        printf("Total Score   = %d\n", total_score);
    }

    void puzzle2(const char* filename) {
        auto inputstream = get_stream<round>(filename);
        auto x = inputstream | stdv::transform(&round::round_result2);
        int total_score = std::accumulate(x.begin(), x.end(), 0);
        printf("Total Score 2 = %d\n", total_score);
    }
} // namespace aoc2022::day02