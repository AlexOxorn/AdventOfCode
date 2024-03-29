#include "../../../common.h"
#include <array>
#include <limits>
#include <ranges>
#include <algorithm>
#include "ox/grid.h"

namespace aoc2021::day05 {
        #define DAY 05
    namespace stdv = std::views;
    namespace stdr = std::ranges;
    constexpr int grid_size = 1000;

    class point : public std::pair<int, int> {
    public:
        int& x = first;
        int& y = second;
    };
    class line : public std::pair<point, point> {
    public:
        point& p1 = first;
        point& p2 = second;

        [[nodiscard]] bool vertical() const { return p1.x == p2.x; }
        [[nodiscard]] bool horizontal() const { return p1.y == p2.y; }
    };

    STREAM_IN(line, l) {
        std::string line;
        std::getline(in, line);
        std::sscanf(line.c_str(), "%d,%d -> %d,%d", &l.p1.x, &l.p1.y, &l.p2.x, &l.p2.y);
        return in;
    }

    class grid : public ox::grid<int, std::array<int, grid_size * grid_size>> {
        int largest = 0;
        bool diag = false;

    public:
        grid() : ox::grid<int, std::array<int, grid_size * grid_size>>(grid_size) {};
        explicit grid(bool b) : ox::grid<int, std::array<int, grid_size * grid_size>>(grid_size), diag(b) {};

        void add_line(const line& l) {
            largest = std::max({largest, l.p1.x, l.p1.y, l.p2.x, l.p2.y});
            if (l.vertical()) {
                auto horizontal = l.p1.x;
                auto [min, max] = std::minmax(l.p1.y, l.p2.y);
                for (int vertical : stdv::iota(min, max+1)) {
                    at(horizontal, vertical)++;
                }
            } else if(l.horizontal()) {
                auto vertical = l.p1.y;
                auto [min, max] = std::minmax(l.p1.x, l.p2.x);
                for (int horizontal : stdv::iota(min, max+1)) {
                    at(horizontal, vertical)++;
                }
            } else if(diag) {
                auto [left, right] = std::minmax(l.p1, l.p2);
                int sign = right.y - left.y;
                int length = std::abs(sign);
                sign /= length;
                for (int i : stdv::iota(0, length+1)) {
                    at(left.x + i, left.y + sign * i)++;
                }
            }
        }

        void print_grid() {
            leveled_foreach(
                    [](auto& elem) { myprintf(elem ? "%d " : ". ", elem); },
                    []() { myprintf("\n"); });
            myprintf("\n");
        }

        [[nodiscard]] auto count_score() const {
            return stdr::count_if(data, [](int x) { return x >= 2; });
        }
    };

    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<line>(filename);
        grid g;
        for (line l : input) {
            g.add_line(l);
        }
        g.print_grid();
        myprintf("the score is: %lu\n", g.count_score());
        return g.count_score();
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<line>(filename);
        grid g(true);
        for (line l : input) {
            g.add_line(l);
        }
        g.print_grid();
        myprintf("the score is: %lu", g.count_score());
        return g.count_score();
    }
}