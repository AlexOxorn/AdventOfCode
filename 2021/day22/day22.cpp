//
// Created by alexoxorn on 2021-12-26.
//

#include "day22.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <execution>

std::hash<int> a();

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
        std::pair<int, int> x;
        std::pair<int, int> y;
        std::pair<int, int> z;
        bool on;
    };

    template <long X0, long X1, long Y0, long Y1, long Z0, long Z1>
    class iterator {
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
        iterator() : current_index(get_full_width()) {}
        explicit iterator(long current_index) : current_index(current_index) {}

        value_type operator*() const {
            return {current_index / get_z_width() / get_y_width() % get_x_width() + X0,
                   current_index / get_z_width() % get_y_width() + Y0,
                   current_index % get_z_width() + Z0};
        }
        iterator& operator++() {
            current_index++;
            return *this;
        }
        iterator operator++(int) {
            auto cpy = *this;
            ++*this;
            return cpy;
        }
        iterator& operator--() {
            current_index--;
            return *this;
        }
        iterator operator--(int) {
            auto cpy = *this;
            --*this;
            return cpy;
        }
        iterator& operator+=(long x) {
            current_index += x;
            return *this;
        }
        iterator operator+(long x) {
            auto cpy = *this;
            cpy += x;
            return cpy;
        }
        iterator& operator-=(long x) {
            current_index -= x;
            return *this;
        }
        iterator operator-(long x) {
            auto cpy = *this;
            cpy -= x;
            return cpy;
        }
        difference_type operator-(const iterator& other) {
            return this->current_index - other.current_index;
        }
        value_type operator[](long x) {
            return *(*this + x);
        }
        auto operator<=>(const iterator& other) const = default;
    };

    std::istream& operator>>(std::istream& in, instruction& inst) {
        std::string s;
        std::getline(in, s);
        char on_off[4] = {};

        sscanf(s.c_str(), "%s x=%d..%d,y=%d..%d,z=%d..%d",on_off,
               &inst.x.first, &inst.x.second,
               &inst.y.first, &inst.y.second,
               &inst.z.first, &inst.z.second);

        inst.x.second++;
        inst.y.second++;
        inst.z.second++;
        inst.on = (std::string("on") == on_off);

        return in;
    }

    void puzzle1() {
        constexpr long width = 50;
        using grid_looper = iterator<-width, width+1, -width, width+1, -width, width+1>;
        auto instructions = GET_STREAM(input, instruction);
        std::vector<instruction> full(instructions.begin(), instructions.end());

        long on_cubes = std::count_if(grid_looper(0), grid_looper(),
               [&full](const cube& c) {
                   for (const instruction& i : stdv::reverse(full)) {
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
        constexpr long width = 100000;
        using grid_looper = iterator<-width, width+1, -width, width+1, -width, width+1>;
        auto instructions = GET_STREAM(sample_input, instruction);
        std::vector<instruction> full(instructions.begin(), instructions.end());

        long on_cubes = std::count_if(std::execution::par_unseq, grid_looper(0), grid_looper(),
               [&full](const cube& c) {
                   auto inst = std::find_if(std::execution::par, full.begin(), full.end(), [&c](const instruction& i) {
                       return i.x.first <= c.x && c.x < i.x.second
                            && i.y.first <= c.y && c.y < i.y.second
                            && i.z.first <= c.z && c.z < i.z.second;
                   });
                   return inst != full.end() && inst->on;
               });
        printf("The number of on cubes is %zu\n", on_cubes);
    }
}