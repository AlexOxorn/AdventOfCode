//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <ranges>

namespace aoc2022::day12 {
    struct heightmap : public ox::grid<char> {
        raw_iterator start;
        raw_iterator end;
        struct mock_end_sentinel {
            friend bool operator==(const raw_iterator& r, const mock_end_sentinel&) { return *r == 'a'; }
        };

        template <typename R>
        explicit heightmap(R&& r) : ox::grid<char>{std::forward<R>(r)} {
            start = stdr::find(data, 'S');
            end = stdr::find(data, 'E');
            *start = 'a';
            *end = 'z';
        }

        auto find_path_part1() {
            auto x = stdr::subrange(start, end);
            return ox::dikstra(
                    start,
                    end,
                    [this](auto curr) -> std::vector<std::pair<heightmap::raw_iterator, int>> {
                        auto neighbours = cardinal_neighbour_range(curr);
                        auto n = neighbours | valid_index()
                               | stdv::filter([&](const auto& x) { return *x <= *curr + 1; })
                               | stdv::transform([](const auto& x) -> std::pair<raw_iterator, int> {
                                     return std::make_pair(x, 1);
                                 });
                        return std::vector(n.begin(), n.end());
                    },
                    [](...) { return 0; },
                    ox::range_iterator_hash<decltype(data)>());
        }

        auto find_path_part2() {
            return ox::dikstra(
                    end,
                    mock_end_sentinel{},
                    [this](auto curr) -> std::vector<std::pair<heightmap::raw_iterator, int>> {
                        auto neighbours = cardinal_neighbour_range(curr);
                        auto n = neighbours | valid_index()
                               | stdv::filter([&](const auto& x) { return *x + 1 >= *curr; })
                               | stdv::transform([](const auto& x) -> std::pair<raw_iterator, int> {
                                     return std::make_pair(x, 1);
                                 });
                        return std::vector(n.begin(), n.end());
                    },
                    [](...) { return 0; },
                    ox::range_iterator_hash<decltype(data)>());
        }
    };

    void puzzle1(const char* filename) {
        heightmap topology(get_stream<ox::line>(filename));
        auto [path, length] = topology.find_path_part1();
        printf("Path Length = %d\n", length);
    }

    void puzzle2(const char* filename) {
        heightmap topology(get_stream<ox::line>(filename));
        auto [path, length] = topology.find_path_part2();
        printf("Min Path Length = %d\n", length);
    }
} // namespace aoc2022::day12