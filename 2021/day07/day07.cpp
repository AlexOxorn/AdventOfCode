//
// Created by alexoxorn on 2021-12-06.
//

#include "day07.h"
#include <limits>
#include <numeric>
#include <ranges>
#include <cmath>

#define YEAR 2021
#define DAY 07

namespace day07 {
    using count_type = long;

    constexpr int triangle_sum(int x) {
        auto y = stdv::iota(1, x+1);
        return std::accumulate(y.begin(), y.end(), 0);
    }

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
            return {std::floor(m/positions.size()), std::ceil(m/positions.size())};
        }

        int calculate_fuel() {
            return calculate_fuel(median());
        }

        int calculate_fuel(int med) {
            auto per_crab_fuel = positions | stdv::transform([med](int a) { return std::abs(med - a); });
            return std::accumulate(per_crab_fuel.begin(), per_crab_fuel.end(), 0);
        }

        int calculate_fuel2(int mean) {
            auto per_crab_fuel = positions | stdv::transform([mean](int a) { return triangle_sum(std::abs(mean - a)); });
            return std::accumulate(per_crab_fuel.begin(), per_crab_fuel.end(), 0);
        }
    };

    void puzzle1() {
        auto input = GET_STREAM(input, int);
        crab_positions cp(input);
        int med = cp.median();
        int fuel = cp.calculate_fuel(med);
        printf("The x position is %d\nThe fuel is %d\n", med, fuel);
    }

    void puzzle2() {
        auto input = GET_STREAM(input, int);
        crab_positions cp(input);
        auto [mean1, mean2] = cp.mean();
        auto [fuel1, fuel2] = std::make_pair(cp.calculate_fuel2(mean1), cp.calculate_fuel2(mean2));
        auto [fuel, mean] = std::min(std::make_pair(fuel1, mean1), std::make_pair(fuel2, mean2));
        printf("The x position is %d\nThe fuel is %d\n", mean, fuel);
    }
}
