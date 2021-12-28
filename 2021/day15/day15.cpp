//
// Created by alexoxorn on 2021-12-15.
//

#include <cassert>
#include <set>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <queue>
#include <cmath>
#include <numeric>
#include <ox/grid.h>
#include <ox/graph.h>
#include "day15.h"

#define YEAR 2021
#define DAY 15

namespace day15 {
    class grid : public ox::grid<int> {
        using ox::grid<int>::grid;

    public:
        explicit grid(std::istream& in, int multiply) {
            std::string s;
            while (std::getline(in, s)) {
                width = s.size() * multiply;
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
        }

        std::pair<std::vector<raw_iterator>, int> find_path() {
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

    void puzzle1() {
        grid g(GET_STREAM(input, ox::line), [](char a) mutable { return a - '0'; });
        auto [path, risk] = g.find_path();
        printf("Total Risk = %d\n", risk);
    }

    void puzzle2() {
        auto input = GET_STREAM(input, ox::line);
        grid g(input, 5);
        auto [path, risk] = g.find_path();
        printf("Total Risk = %d\n", risk);
    }
} // namespace day15
