#include "../../../common.h"
#include <ox/grid.h>
#include <cstdio>
#include <array>
#include <numeric>

namespace aoc2020::day03 {
    int count_trees(const ox::grid<bool>& forest, int delta_x, int delta_y) {
        int x = 0;
        int height = static_cast<int>(forest.get_height());
        int width = static_cast<int>(forest.get_width());
        int sum = 0;
        for (int y = 0; y < height; y += delta_y, x = (x + delta_x) % width) {
            sum += forest.at(x, y);
        }
        return sum;
    }

    ox::grid<bool>& get_forest(puzzle_options filename) {
        static ox::grid<bool> forest{get_stream<>(filename), [](char c) { return c == '#'; }};
        return forest;
    }


    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<>(filename);
        int result = count_trees(get_forest(filename), 3, 1);
        printf("The number of trees on the 3/1 diagonal is %d\n", result);
        return result;
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<>(filename);
        auto& forest = get_forest(filename);
        std::array directions{
                std::pair(1, 1),
                std::pair(3, 1),
                std::pair(5, 1),
                std::pair(7, 1),
                std::pair(1, 2),
        };
        auto tree_counts = directions | stdv::transform([&](std::pair<int, int> dir) {
                              return count_trees(forest, dir.first, dir.second);
                          });
        int total = std::accumulate(tree_counts.begin(), tree_counts.end(), 1, std::multiplies{});
        printf("The sum of trees on all directions is %d\n", total);
        return total;
    }
} // namespace aoc2020::day03