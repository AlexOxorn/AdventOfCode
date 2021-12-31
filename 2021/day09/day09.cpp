//
// Created by alexoxorn on 2021-12-10.
//

#include "day09.h"
#include "algorithms/_helper_iterators.h"
#include <vector>
#include <numeric>
#include <queue>
#include <unordered_set>
#include <ranges>
#include <ox/grid.h>
#include <ox/ranges.h>
#include <ox/debug.h>

#define YEAR 2021
#define DAY 09

namespace day09 {

    class heightmap : public ox::grid<int> {
        using ox::grid<int>::grid;
    public:
        bool is_low_point(const_raw_iterator index) {
            int value = *index;
            auto neighbours = cardinal_neighbour_range(index);
            auto valid_neighbours = neighbours | const_valid_index();
            return stdr::all_of(valid_neighbours, [value](auto a) { return *a > value; });
        }

        int get_basin_size(const_raw_iterator index) {
            std::unordered_set<const_raw_iterator, ox::iterator_hash<const_raw_iterator>> indices_in_basin{index};
            std::queue<const_raw_iterator> dfs;
            dfs.push(index);
            while(!dfs.empty()) {
                auto current = dfs.front();
                dfs.pop();
                indices_in_basin.insert(current);
                auto neighbours = cardinal_neighbour_range(current);
                for ( const_raw_iterator neighbour : neighbours
                                                | const_valid_index()
                                                | stdv::filter([](const_raw_iterator n) { return *n != 9; })
                    ) {
                    if (!indices_in_basin.contains(neighbour)) dfs.push(neighbour);
                }
            }
            return indices_in_basin.size();
        }

        int get_score() {
            auto x = data
                     | ox::ranges::views::iterators
                     | stdv::filter([this] (auto x) { return is_low_point(x); })
                     | stdv::transform([this] (auto x) { return *x + 1; });
            return std::accumulate(x.begin(), x.end(), 0);
        }

        int get_score2() {
            auto x = data
                     | ox::ranges::views::iterators
                     | stdv::filter([this] (auto x) { return is_low_point(x); })
                     | stdv::transform([this] (auto x) { return get_basin_size(x); });
            std::vector basin_sizes(x.begin(), x.end());
            std::nth_element(basin_sizes.begin(), basin_sizes.begin() + 3, basin_sizes.end(), std::greater<int>{});
            return std::accumulate(basin_sizes.begin(), basin_sizes.begin() + 3, 1, std::multiplies<int>());
            return 0;
        }

        void print_array() {
            leveled_iterators(
                    [this](auto& elem) {
                        printf("\033[%sm%1d\033[0m", is_low_point(elem) ? "31" : *elem == 9 ? "30" : "0", *elem);
                    },
                    []() { printf("\n"); });
            printf("\n");
        }
    };

    void puzzle1() {
        heightmap h(GET_STREAM(input, ox::line), [](char a) {return a - '0';});
        auto [x, y] = h.get_dimensions();
        printf("size = %zu x %zu\n", x, y);
        h.print_array();
        printf("score = %d\n", h.get_score());
    }

    void puzzle2() {
        heightmap h(GET_STREAM(input, ox::line), [](char a) {return a - '0';});
        printf("score = %d\n", h.get_score2());
    }
}