//
// Created by alexoxorn on 2021-12-26.
//

#include "day22.h"
#include <functional>
#include <numeric>
#include <cmath>
#include <ox/ranges.h>

#define YEAR 2021
#define DAY 22

namespace day22 {
    struct cube {
        long x;
        long y;
        long z;

        auto operator<=>(const cube& other) const = default;
    };

    struct instruction {
        std::pair<double, double> x;
        std::pair<double, double> y;
        std::pair<double, double> z;
        bool on;
    };

    template <long X0, long X1, long Y0, long Y1, long Z0, long Z1>
    class three_dimensional_index_iterator {
    public:
        using value_type = cube;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = long;
    private:
        long current_index{};

        [[nodiscard]] long get_z_width() const { return Z1 - Z0; }
        [[nodiscard]] long get_y_width() const { return Y1 - Y0; }
        [[nodiscard]] long get_x_width() const { return X1 - X0; }
        [[nodiscard]] long get_full_width() const { return get_z_width() * get_y_width() * get_x_width(); }
    public:
        three_dimensional_index_iterator() : current_index(get_full_width()) {}
        explicit three_dimensional_index_iterator(long current_index) : current_index(current_index) {}

        value_type operator*() const {
            return {current_index / get_z_width() / get_y_width() % get_x_width() + X0,
                   current_index / get_z_width() % get_y_width() + Y0,
                   current_index % get_z_width() + Z0};
        }
        three_dimensional_index_iterator& operator++() {
            current_index++;
            return *this;
        }
        three_dimensional_index_iterator operator++(int) {
            auto cpy = *this;
            ++*this;
            return cpy;
        }
        three_dimensional_index_iterator& operator--() {
            current_index--;
            return *this;
        }
        three_dimensional_index_iterator operator--(int) {
            auto cpy = *this;
            --*this;
            return cpy;
        }
        three_dimensional_index_iterator& operator+=(long x) {
            current_index += x;
            return *this;
        }
        three_dimensional_index_iterator operator+(long x) {
            auto cpy = *this;
            cpy += x;
            return cpy;
        }
        three_dimensional_index_iterator& operator-=(long x) {
            current_index -= x;
            return *this;
        }
        three_dimensional_index_iterator operator-(long x) {
            auto cpy = *this;
            cpy -= x;
            return cpy;
        }
        difference_type operator-(const three_dimensional_index_iterator& other) {
            return this->current_index - other.current_index;
        }
        value_type operator[](long x) {
            return *(*this + x);
        }
        auto operator<=>(const three_dimensional_index_iterator& other) const = default;
    };

    std::istream& operator>>(std::istream& in, instruction& inst) {
        std::string s;
        std::getline(in, s);
        char on_off[4] = {};

        int x0, x1, y0, y1, z0, z1;

        sscanf(s.c_str(), "%s x=%d..%d,y=%d..%d,z=%d..%d",on_off,
               &x0, &x1, &y0, &y1, &z0, &z1);

        inst.x.first = static_cast<double>(x0) - 0.5;
        inst.y.first = static_cast<double>(y0) - 0.5;
        inst.z.first = static_cast<double>(z0) - 0.5;
        inst.x.second = static_cast<double>(x1) + 0.5;
        inst.y.second = static_cast<double>(y1) + 0.5;
        inst.z.second = static_cast<double>(z1) + 0.5;

        inst.on = (std::string("on") == on_off);

        return in;
    }

    template <typename Proj>
    std::vector<double> list_of_boundaries(const std::vector<instruction>& inst, const Proj& p) {
        std::vector<double> to_return;
        to_return.reserve(inst.size() * 2);
        for (const instruction& c : inst) {
            auto x = c.*p;
            to_return.push_back(x.first);
            to_return.push_back(x.second);
        }

        std::sort(to_return.begin(), to_return.end());
        auto end_unique = std::unique(to_return.begin(), to_return.end());
        to_return.erase(end_unique, to_return.end());
        return to_return;
    }

    static std::vector<instruction> instructions;
    static std::vector<double> x_boundaries;
    static std::vector<double> y_boundaries;
    static std::vector<double> z_boundaries;

    void puzzle1() {
        constexpr long width = 50;
        using grid_looper = three_dimensional_index_iterator<-width, width+1, -width, width+1, -width, width+1>;
        auto input = GET_STREAM(input, instruction);
        instructions = std::vector(input.begin(), input.end());

        x_boundaries = list_of_boundaries(instructions, &instruction::x);
        y_boundaries = list_of_boundaries(instructions, &instruction::y);
        z_boundaries = list_of_boundaries(instructions, &instruction::z);

        long on_cubes = std::count_if(grid_looper(0), grid_looper(),
               [](const cube& c) {
                   for (const instruction& i : stdv::reverse(instructions)) {
                       if (i.x.first <= c.x && c.x < i.x.second
                              && i.y.first <= c.y && c.y < i.y.second
                              && i.z.first <= c.z && c.z < i.z.second)
                           return i.on;
                   }
                   return false;
               });
        printf("The number of on cubes after initialization is %zu\n", on_cubes);
    }

    void puzzle2() {
        long on_cubes = 0;

        for(auto x_iter : x_boundaries | ox::ranges::views::iterators) {
            printf("%ld out of %zu\n", x_iter - x_boundaries.begin(), x_boundaries.size());
            if (std::next(x_iter) == x_boundaries.end()) break;
            for (auto y_iter : y_boundaries | ox::ranges::views::iterators) {
                if (std::next(y_iter) == y_boundaries.end()) continue;
                for (auto z_iter : z_boundaries | ox::ranges::views::iterators) {
                    if (std::next(z_iter) == z_boundaries.end()) continue;
                    double x = *x_iter + 0.5;
                    double y = *y_iter + 0.5;
                    double z = *z_iter + 0.5;
                    for (const instruction& i : stdv::reverse(instructions)) {
                        if (i.x.first <= x && x <= i.x.second
                               && i.y.first <= y && y <= i.y.second
                               && i.z.first <= z && z <= i.z.second) {
                            on_cubes += i.on * std::lround((x_iter[1] - *x_iter)
                                               * (y_iter[1] - *y_iter)
                                               * (z_iter[1] - *z_iter));
                            break;
                        }
                    }
                }
            }
        }
        printf("The number of on cubes is %zu\n", on_cubes);
    }
}