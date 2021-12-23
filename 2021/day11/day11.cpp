//
// Created by alexoxorn on 2021-12-11.
//

#include "day11.h"

#define YEAR 2021
#define DAY 11

#include <vector>
#include <set>
#include <queue>
#include <numeric>
#include <optional>
#include <concepts>
#include <ox/grid.h>

namespace day11 {
    class octopuses : public ox::grid<int> {
        using ox::grid<int>::grid;
    public:
        auto get_size() {
            return data.size();
        }

        int next_step() {
            std::set<raw_iterator> indices_flashed{};
            std::queue<raw_iterator> dfs;

            for(auto& x : data | ox::ranges::views::iterators()) {
                if(++(*x) > 9) {
                    dfs.push(x);
                }
            }

            while (!dfs.empty()) {
                raw_iterator current = dfs.front();
                dfs.pop();
                auto [in_iter, success] = indices_flashed.insert(current);
                if (!success) {
                    continue;
                }

                auto neighbour = neighbour_range(current);
                for(auto index : neighbour | valid_index()) {
                    if (++(*index) > 9)
                        dfs.push(index);
                }
            }

            for (auto x : indices_flashed) {
                *x = 0;
            }

            return indices_flashed.size();
        }

        void print_array() {
            leveled_foreach(
                    [](int i) { printf("\033[%sm%4d\033[0m", i == 0 || i > 9 ? "31" : "0", i); },
                    []() { printf("\n"); }
            );
        }
    };

    void puzzle1() {
        int number_of_steps = 100;
        octopuses o(GET_STREAM(input, ox::line), [](char a) {return a - '0';});
        auto scores = stdv::iota(0, number_of_steps) | stdv::transform([&o] (int) { return o.next_step(); });
        printf("Number of flashes after %d steps: %d\n", number_of_steps, std::accumulate(scores.begin(), scores.end(), 0));
    }

    void puzzle2() {
        octopuses o(GET_STREAM(input, ox::line), [](char a) {return a - '0';});
        auto scores = stdv::iota(1)
                | stdv::transform([&o] (int a) { return std::make_pair(a, o.next_step()); })
                | stdv::filter([&o](std::pair<int, int> a) { return std::size_t(a.second) == o.get_size(); })
                | stdv::take(1);
        printf("first point when all octopuses flash: %d\n", (*scores.begin()).first);
    }
}
