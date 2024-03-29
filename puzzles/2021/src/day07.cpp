#include "../../../common.h"
#include <limits>
#include <numeric>
#include <ranges>
#include <cmath>
#include <algorithm>
#include "ox/math.h"

#define DAY 07

namespace aoc2021::day07 {
    class crab_positions {
        std::vector<int> positions{};
    public:
        explicit crab_positions(std::istream& in) {
            int i;
            while(in >> i) {
                positions.push_back(i);
                in.ignore(std::numeric_limits<std::streamsize>::max(), ',');
            }
        }

        int median() {
            auto m = positions.begin() + positions.size()/2;
            std::nth_element(positions.begin(), m, positions.end());
            return positions[positions.size()/2];
        }

        std::pair<int, int> mean() {
            auto m = std::accumulate(positions.begin(), positions.end(), 0.0);
            auto mean_value = m / positions.size();
            return {std::floor(mean_value), std::ceil(mean_value)};
        }

        int calculate_fuel() {
            return calculate_fuel(median());
        }

        int calculate_fuel(int med) {
            auto per_crab_fuel = positions | stdv::transform([med](int a) { return std::abs(med - a); });
            return std::accumulate(per_crab_fuel.begin(), per_crab_fuel.end(), 0);
        }

        int calculate_fuel2(int mean) {
            auto per_crab_fuel = positions | stdv::transform([mean](int a) { return ox::triangle_sum(std::abs(mean - a)); });
            return std::accumulate(per_crab_fuel.begin(), per_crab_fuel.end(), 0);
        }
    };

    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<int>(filename);
        crab_positions cp(input);
        int med = cp.median();
        int fuel = cp.calculate_fuel(med);
        myprintf("The x position is %d\nThe fuel is %d\n", med, fuel);
        return fuel;
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<int>(filename);
        crab_positions cp(input);
        auto [mean1, mean2] = cp.mean();
        auto [fuel1, fuel2] = std::make_pair(cp.calculate_fuel2(mean1), cp.calculate_fuel2(mean2));
        auto [fuel, mean] = std::min(std::make_pair(fuel1, mean1), std::make_pair(fuel2, mean2));
        myprintf("The x position is %d\nThe fuel is %d\n", mean, fuel);
        return fuel;
    }
}
