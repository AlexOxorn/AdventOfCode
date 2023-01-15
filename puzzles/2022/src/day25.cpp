//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <numeric>

namespace aoc2022::day25 {
    long from_snafu_char(char c) {
        switch (c) {
            case '1': return 1l;
            case '2': return 2l;
            case '0': return 0l;
            case '-': return -1l;
            case '=': return -2l;
            default: throw std::exception();
        }
    }

    long from_snafu(const std::string& s) {
        return std::accumulate(s.begin(), s.end(), 0l, [](long x, char c) { return x * 5 + from_snafu_char(c); });
    }

    std::string to_base5_string(long x) {
        std::string s;
        while (x) {
            s.push_back(static_cast<char>('0' + (x % 5)));
            x /= 5;
        }
        stdr::reverse(s);
        return s;
    }

    std::pair<char, char> to_snafu(char c) {
        char mod = 0;
        while (c >= '3') {
            c -= 5;
            ++mod;
        }
        switch (c) {
            case '0': return {mod, '0'};
            case '1': return {mod, '1'};
            case '2': return {mod, '2'};
            case '0' - 2: return {mod, '='};
            case '0' - 1: return {mod, '-'};
            default: throw std::exception();
        }
    }

    void base5_to_snafu(std::string& s) {
        auto head = s.rbegin();
        auto last = s.rend();
        auto second_last = last - 1;
        for (; head != last; ++head) {
            char& digit = *head;
            auto [carry, remaining] = to_snafu(digit);
            digit = remaining;
            if (head == second_last) {
                s.insert(s.begin(), static_cast<char>(carry + '0'));
                return;
            }
            head[1] = static_cast<char>(head[1] + carry);
        }
    }

    std::string to_snafu(long x) {
        std::string s = to_base5_string(x);
        base5_to_snafu(s);
        return s;
    }

    void puzzle1(const char* filename) {
        auto input = get_stream<ox::line>(filename);

        auto values = input | stdv::transform(from_snafu);
        auto result_int = std::accumulate(values.begin(), values.end(), 0l);
        std::string result_snafu = to_snafu(result_int);
        printf("The resulting sum is %ld in decimal and %s in snafu\n", result_int, result_snafu.c_str());
    }

    void puzzle2([[maybe_unused]] const char* filename) {}
} // namespace aoc2022::day25