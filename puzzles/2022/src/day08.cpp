//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace aoc2022::day08 {
    struct tree_visibility : public std::pair<int, bool> {
        using std::pair<int, bool>::pair;
        tree_visibility(int i) : std::pair<int, bool>{i, false} {};
    };

    struct treemap : public ox::grid<tree_visibility> {
        using ox::grid<tree_visibility>::grid;
        using direction_func = std::optional<const_raw_iterator> (treemap::*)(std::optional<const_raw_iterator>) const;

        template <typename T>
        void check_row(raw_iterator begin, raw_iterator end, T view) {
            int current_height = -1;
            for (auto& tree : stdr::subrange(begin, end) | view) {
                if (tree.first > current_height) {
                    tree.second = true;
                    current_height = tree.first;
                }
            }
        }

        void check_horizontal() {
            for (auto [begin, end] : *this) {
                check_row(begin, end, stdv::all);
                check_row(begin, end, stdv::reverse);
            }
        }

        template <typename T>
        void check_column(int column, T view) {
            int current_height = -1;
            for (size_t i : stdv::iota(size_type(0), get_height()) | view) {
                auto& tree = this->at(column, i);
                if (tree.first > current_height) {
                    tree.second = true;
                    current_height = tree.first;
                }
            }
        }

        void check_vertical() {
            for (int j = 0; j < this->width; j++) {
                check_column(j, stdv::all);
                check_column(j, stdv::reverse);
            }
        }

        void print_map() {
            leveled_iterators([](auto& elem) { printf("\033[%sm%1d\033[0m", elem->second ? "31" : "0", elem->first); },
                              []() { printf("\n"); });
        }

        [[nodiscard]] long get_tree_score_dir(const_raw_iterator tree, direction_func dir) const {
            int count = 0;
            for (auto cur = (this->*dir)(std::optional(tree)); cur; cur = (this->*dir)(cur)) {
                count++;
                if ((*cur)->first >= tree->first)
                    break;
            }
            return count;
        }

        [[nodiscard]] long get_tree_score(const_raw_iterator tree) const {
            direction_func dirs[4] = {&treemap::up, &treemap::left, &treemap::down, &treemap::right};
            auto scores = dirs | stdv::transform([&](auto a) { return get_tree_score_dir(tree, a); });
            return std::accumulate(scores.begin(), scores.end(), 1l, std::multiplies<>());
        }
    };

    treemap get_marked_tree(const char* filename) {
        treemap trees(get_stream<ox::line>(filename), [](char a) { return a - '0'; });
        trees.check_horizontal();
        trees.check_vertical();
        return trees;
    }

    void puzzle1(const char* filename) {
        treemap trees = get_marked_tree(filename);
        trees.print_map();
        long count = stdr::count_if(trees.get_raw(), &std::pair<int, bool>::second);
        printf("There are %ld number of visible trees\n", count);
    }

    void puzzle2(const char* filename) {
        treemap trees = get_marked_tree(filename);
        long result = stdr::max(trees.get_raw() | oxv::iterators
                                | stdv::filter([](const treemap::const_raw_iterator& tree) { return tree->second; })
                                | stdv::transform([&trees](const treemap::const_raw_iterator& tree) {
                                      return trees.get_tree_score(tree);
                                  }));
        printf("The best Treehouse spot has a score of %ld\n", result);
    }
} // namespace aoc2022::day08