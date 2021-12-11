//
// Created by alexoxorn on 2021-12-06.
//

#include "day06.h"
#include <limits>
#include <numeric>
#include <ranges>
#include <array>

#define YEAR 2021
#define DAY 06

namespace day06 {
    using count_type = long;

    class fish_population {
        std::array<count_type, 9> days_count{};
        int day = 0;
    public:
        explicit fish_population(std::istream& in) {
            int i;
            while(in >> i) {
                days_count.at(i) += 1;
                in.ignore(std::numeric_limits<std::streamsize>::max(), ',');
            }
        }

        fish_population& operator++() {
            day++;
            std::rotate(days_count.begin(), days_count.begin() + 1, days_count.end());
            days_count.at(6) += days_count.at(8);
            return *this;
        }

        void print_state() {
            switch (day) {
                case 0:
                    printf("Initial State:  ");
                    break;
                case 1:
                    printf("After   1 day:  ");
                    break;
                default:
                    printf("After %3d days: ", day);
            }
            std::cout << std::accumulate(days_count.begin(), days_count.end(), count_type{});
            printf(" Total\n");
        }
    };

    void run_test(int generations) {
        auto input = GET_STREAM(input, int);
        fish_population fishes(input);
        for(int a : stdv::iota(0, generations)) {
            ++fishes;
        }
        fishes.print_state();
    }

    void puzzle1() {
        run_test(80);
    }

    void puzzle2() {
        run_test(256);
    }
}
