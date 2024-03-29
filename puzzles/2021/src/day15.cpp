#include <cassert>
#include <set>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <queue>
#include <cmath>
#include <numeric>
#include "ox/grid.h"
#include "ox/graph.h"
#include "../../../common.h"

#define DAY 15

namespace aoc2021::day15 {
    class grid : public ox::grid<int> {
        using ox::grid<int>::grid;

    public:
        explicit grid(std::istream& in, int multiply) {
            std::string s;
            while (std::getline(in, s)) {
                for (int offset : stdv::iota(0, multiply)) {
                    std::transform(s.begin(), s.end(), std::back_inserter(data), [offset](char a) {
                        return (a - '0' + offset) % 10 + (a - '0' + offset) / 10;
                    });
                }
            }
            data.reserve(multiply * data.size());
            auto initial_end = data.end();
            for (int offset : stdv::iota(1, multiply)) {
                std::transform(data.begin(), initial_end, std::back_inserter(data), [offset](int a) {
                    return (a + offset) % 10 + (a + offset) / 10;
                });
            }
            set_width(s.size() * multiply);
        }

        auto find_path() {
            return ox::dikstra(
                   data.begin(), data.end() - 1,
                   [this](auto curr) -> std::vector<std::pair<raw_iterator, int>> {
                       auto neighbours = cardinal_neighbour_range(curr);
                       auto n = neighbours
                              | valid_index()
                              | stdv::transform([](const auto& x) -> std::pair<raw_iterator, int> {
                                    return std::make_pair(x, *x);
                                });
                       return std::vector(n.begin(), n.end());
                   },
                   [](...) { return 0; },
                   ox::range_iterator_hash<decltype(data)>());
        }
    };

    answertype puzzle1(puzzle_options filename) {
        grid g(get_stream<ox::line>(filename), [](char a) mutable { return a - '0'; });
        auto [path, risk] = g.find_path();
        myprintf("Total Risk = %d\n", risk);
        return risk;
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<ox::line>(filename);
        grid g(input, 5);
        auto [path, risk] = g.find_path();
        myprintf("Total Risk = %d\n", risk);
        return risk;
    }
} // namespace aoc2021::day15
