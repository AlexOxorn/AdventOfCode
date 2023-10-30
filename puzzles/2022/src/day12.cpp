//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <ranges>

namespace aoc2022::day12 {
    struct heightmap : public ox::grid<char> {
        using position = raw_iterator;
        position start;
        position end;
        struct mock_end_sentinel {
            friend bool operator==(const position& r, const mock_end_sentinel&) { return *r == 'a'; }
        };

        template <typename R>
        explicit heightmap(R&& r) : ox::grid<char>{std::forward<R>(r)} {
            start = stdr::find(data, 'S');
            end = stdr::find(data, 'E');
            *start = 'a';
            *end = 'z';
        }

        template <typename End, typename Filter>
        auto find_path(position start, End end, Filter neighbour_filter) {
            return ox::dikstra(
                    start,
                    end,
                    [neighbour_filter, this](auto curr) -> std::vector<std::pair<position, int>> {
                        auto neighbours = this->cardinal_neighbour_range(curr);
                        auto n = neighbours | valid_index()
                               | stdv::filter([&](const auto& x) { return neighbour_filter(curr, x); })
                               | stdv::transform([](const auto& x) -> std::pair<position, int> {
                                     return std::make_pair(x, 1);
                                 });
                        return std::vector(n.begin(), n.end());
                    },
                    [](...) { return 0; },
                    ox::range_iterator_hash<decltype(data)>());
        }

        auto find_path_part1() {
            return find_path(start, end, [](position curr, position x) { return *x <= *curr + 1; });
        }

        auto find_path_part2() {
            return find_path(end, mock_end_sentinel{}, [](position curr, position x) { return *x + 1 >= *curr; });
        }
    };

    answertype puzzle1(const char* filename) {
        heightmap topology(get_stream<ox::line>(filename));
        auto [path, length] = topology.find_path_part1();
        myprintf("Path Length = %d\n", length);
        return length;
    }

    answertype puzzle2(const char* filename) {
        heightmap topology(get_stream<ox::line>(filename));
        auto [path, length] = topology.find_path_part2();
        myprintf("Min Path Length = %d\n", length);
        return length;
    }
} // namespace aoc2022::day12