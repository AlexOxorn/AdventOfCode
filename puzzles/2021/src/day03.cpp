#include "../../../common.h"
#include <array>
#include <sstream>
#include <numeric>
#include <algorithm>
#include <bitset>
#include "ox/math.h"
#include "ox/utils.h"

#define DAY 03

namespace aoc2021::day03{
    constexpr size_t bitwidth = 12;

    class reading : public ox::bitset_container<bitwidth> {};

    class bit_population_count {
        std::vector<int> part = std::vector<int>(bitwidth);
        int total_count = 0;
    public:
        bit_population_count& operator+(const reading& r) {
            std::transform(r.begin(), r.end(), part.begin(), part.begin(), std::plus<>( ));
            total_count++;
            return *this;
        }

        [[nodiscard]] unsigned common_to_integer() const {
            reading r;
            int half = (total_count+1)/2;
            std::transform(part.begin(), part.end(), r.begin(), [half](int i) { return i > half; });
            return r.to_ulong();
        }
    };

    std::pair<int, int> gamma_epsilon_rate(unsigned long l, size_t width = bitwidth) {
        return {l, (~l) % (1 << width)};
    }

    std::vector<int> operator+(std::vector<int> total, const reading& g) {
        total.at(g.to_ulong()) += 1;
        return total;
    }

    template <std::random_access_iterator iter, typename Compare = std::greater<int>>
    int recursive_decent(const iter true_begin, iter end, Compare c = Compare()) {
        std::random_access_iterator auto begin = true_begin;
        long dist;
        while((dist = std::distance(begin, end)) > 1) {
            iter mid = begin + dist / 2;
            auto sum1 = std::accumulate(begin, mid, 0);
            auto sum2 = std::accumulate(mid, end, 0);
            if (sum1 + sum2 == 1) {
                return std::find(begin, end, 1) - true_begin;
            }
            (c(sum1, sum2) ? end : begin) = mid;
        }
        return begin - true_begin;
    }

    answertype puzzle1(puzzle_options filename) {
        auto input_stream = get_stream<reading>(filename);
        auto final = std::accumulate(input_stream.begin(), input_stream.end(), bit_population_count());
        auto[gamma, epsilon] = gamma_epsilon_rate(final.common_to_integer());
        myprintf("gamma is %d and epsilon is %d\n", gamma, epsilon);
        myprintf("their product is: %d\n", gamma * epsilon);
        return std::to_string(gamma * epsilon);
    }

    answertype puzzle2(puzzle_options filename) {
        auto input_stream = get_stream<reading>(filename);
        auto binary_tree = std::accumulate(input_stream.begin(), input_stream.end(), std::vector<int>(ox::power_of_2(bitwidth)));
        int o2 = recursive_decent(binary_tree.begin(), binary_tree.end());
        int co2 = recursive_decent(binary_tree.begin(), binary_tree.end(), std::less_equal<>());
        myprintf("O2  is %d\n", o2);
        myprintf("CO2 is %d\n", co2);
        myprintf("Their product is %d\n", co2 * o2);
        return std::to_string(co2 * o2);
    }
}
