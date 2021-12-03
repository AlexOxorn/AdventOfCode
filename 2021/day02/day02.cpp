//
// Created by alexoxorn on 2021-12-02.
//

#include "day02.h"
#include <sstream>
#include <vector>
#include <optional>
#include <numeric>
#include <tuple>
#include <ranges>

#define YEAR 2021
#define DAY 02

namespace day02 {
    struct direction {
        std::string dir;
        int length;
    };
    using coordinates = std::tuple<int, int, int>;

    std::istream& operator>>(std::istream& in, direction& out) {
        return in >> out.dir >> out.length;
    }

    DEFINE_DEFAULT_GET_VECTORS(direction)

    void print_result(coordinates c) {
        auto [forward, depth, aim] = c;
        printf("Coordinates are %d forward and %d down\n", forward, depth);
        printf("Their product is %d\n", forward * depth);
    }

    coordinates move_version1(coordinates c, const direction& d) {
        auto& [forward, depth, aim] = c;
        const auto& [dir, length] = d;
        if (dir == "forward")
            forward += length;
        if (dir == "down")
            depth += length;
        if (dir == "up")
            depth -= length;
        return c;
    }

    coordinates move_version2(coordinates c, const direction& d) {
        auto& [forward, depth, aim] = c;
        const auto& [dir, length] = d;
        if (dir == "forward") {
            forward += length;
            depth += aim * length;
        }
        if (dir == "down")
            aim += length;
        if (dir == "up")
            aim -= length;
        return c;
    }

    template <std::ranges::range R>
    coordinates follow_path(R& vd, coordinates(*move_function)(coordinates, const direction&)) {
        return std::accumulate(vd.begin(), vd.end(), coordinates(), move_function);
    }

    void puzzle1() {
        auto input_vector = GET_STREAM(input, direction);
        auto result = follow_path(input_vector, move_version1);
        print_result(result);
    }

    void puzzle2() {
        auto input_vector = GET_STREAM(input, direction);
        auto result = follow_path(input_vector, move_version2);

        print_result(result);
    }
}