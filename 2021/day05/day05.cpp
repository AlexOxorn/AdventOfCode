//
// Created by alexoxorn on 2021-12-04.
//

#include "day05.h"
#include <array>
#include <limits>
#include <ranges>
#include <algorithm>

namespace day05 {
    #define YEAR 2021
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

        [[nodiscard]] bool vertical() const {
            return p1.x == p2.x;
        }

        [[nodiscard]] bool horizontal() const {
            return p1.y == p2.y;
        }
    };

    std::istream& operator>>(std::istream& in, point& p) {
        in >> p.x;
        in.ignore(std::numeric_limits<std::streamsize>::max(), ',');
        in >> p.y;
        return in;
    }

    std::istream& operator>>(std::istream& in, line& p) {
        in >> p.p1;
        in.ignore(std::numeric_limits<std::streamsize>::max(), '-');
        in.ignore(std::numeric_limits<std::streamsize>::max(), '>');
        in >> p.p2;
        return in;
    }

    class grid {
        std::array<int, grid_size * grid_size> data{};
        int largest = 0;
        bool diag = false;

        [[nodiscard]] const int& get(int i, int j) const {
            return data[j * grid_size + i];
        }

        [[nodiscard]] int& get(int i, int j) {
            return data[i * grid_size + j];
        }
    public:
        grid() = default;
        explicit grid(bool b) : diag(b) {};

        void add_line(const line& l) {
            largest = std::max({largest, l.p1.x, l.p1.y, l.p2.x, l.p2.y});
            if (l.vertical()) {
                auto horizontal = l.p1.x;
                auto [min, max] = std::minmax(l.p1.y, l.p2.y);
                for (int vertical : stdv::iota(min, max+1)) {
                    get(horizontal, vertical)++;
                }
            } else if(l.horizontal()) {
                auto vertical = l.p1.y;
                auto [min, max] = std::minmax(l.p1.x, l.p2.x);
                for (int horizontal : stdv::iota(min, max+1)) {
                    get(horizontal, vertical)++;
                }
            } else if(diag) {
                auto [left, right] = std::minmax(l.p1, l.p2);
                int sign = right.y - left.y;
                int length = std::abs(sign);
                sign /= length;
                for (int i : stdv::iota(0, length+1)) {
                    get(left.x + i, left.y + sign * i)++;
                }
            }
        }

        void print_grid(int max = 100) const {
            int size = std::min(max, largest);
            for (int x : stdv::iota(0, size+1)) {
                for (int y : stdv::iota(0, size+1)) {
                    int elem = get(x, y);
                    printf(elem ? "%d " : ". ", elem);
                }
                printf("\n");
            }
        }

        [[nodiscard]] auto count_score() const {
            return stdr::count_if(data, [](int x) { return x >= 2; });
        }
    };

    void puzzle1() {
        auto input = GET_STREAM(input, line);
        grid g;
        for (line l : input) {
            g.add_line(l);
        }
        g.print_grid();
        printf("the score is: %lu\n", g.count_score());
    }

    void puzzle2() {
        auto input = GET_STREAM(input, line);
        grid g(true);
        for (line l : input) {
            g.add_line(l);
        }
        g.print_grid();
        printf("the score is: %lu", g.count_score());
    }
}