#include "../../../common.h"
#include <ox/grid.h>
#include <ranges>
#include <unordered_set>

namespace aoc2023::day11 {
    struct universe : ox::grid<char> {
        using ox::grid<char>::grid;
        std::unordered_set<long> empty_cols;
        std::unordered_set<long> empty_rows;

        void print() {
            for (long x = 0; x < dimensions[0]; ++x) {
                for (long y = 0; y < dimensions[1]; ++y) {
                    if (empty_row(y) || empty_column(x)) {
                        myprintf("\033[31m%c", at(x, y));
                    } else {
                        myprintf("\033[0m%c", at(x, y));
                    }
                }
                myprintf("\n");
            }
        }

        void find_empty_columns() {
            for (long col_index = 0; col_index < dimensions[1]; ++col_index) {
                for (long row = 0; row < dimensions[0]; ++row) {
                    if (at(col_index, row) == '#') {
                        goto outer_continue;
                    }
                }
                empty_cols.insert(col_index);
            outer_continue:
            }
            for (long row_index = 0; row_index < dimensions[0]; ++row_index) {
                for (long col = 0; col < dimensions[1]; ++col) {
                    if (at(col, row_index) == '#') {
                        goto outer_continue2;
                    }
                }
                empty_rows.insert(row_index);
            outer_continue2:
            }
        }

        bool empty_column(long col_index) const { return empty_cols.contains(col_index); }
        bool empty_row(long row_index) const { return empty_rows.contains(row_index); }

        long count_empty_columns(long start, long end) const {
            long count = 0;
            for (; start < end; ++start) {
                count += empty_column(start);
            }
            return count;
        }
        long count_empty_row(long start, long end) const {
            long count = 0;
            for (; start < end; ++start) {
                count += empty_row(start);
            }
            return count;
        }

        std::vector<const_raw_iterator> get_galaxies() const {
            std::vector<const_raw_iterator> to_return;
            auto head = data.begin();
            while (true) {
                head = std::find(head, data.end(), '#');
                if (head == data.end())
                    return to_return;
                to_return.push_back(head);
                ++head;
            }
        }

        template <long expansion>
        long get_distance(const_raw_iterator start, const_raw_iterator end) const {
            auto [x1, y1] = coord_from_index(start);
            auto [x2, y2] = coord_from_index(end);
            if (x2 < x1)
                std::swap(x2, x1);
            if (y2 < y1)
                std::swap(y2, y1);
            auto horizontal_distance = x2 - x1 + (expansion - 1) * count_empty_columns(x1, x2);
            auto vertical_distance = y2 - y1 + (expansion - 1) * count_empty_row(y1, y2);
            return horizontal_distance + vertical_distance;
        }

        template <long expansion>
        long get_pairwise_distances() const {
            auto galaxies = get_galaxies();
            long distances = 0;
            for (long x = 0; x < long(galaxies.size() - 1); ++x) {
                for (long y = x + 1; y < long(galaxies.size()); y++) {
                    distances += get_distance<expansion>(galaxies[x], galaxies[y]);
                }
            }
            return distances;
        }
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto stream = get_stream(filename);
        universe u(stream, std::identity());
        u.find_empty_columns();
        long res = u.get_pairwise_distances<2>();
        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto stream = get_stream(filename);
        universe u(stream, std::identity());
        u.find_empty_columns();
        long res = u.get_pairwise_distances<1'000'000>();
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day11