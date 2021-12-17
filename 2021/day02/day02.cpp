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
    enum class direction {
        forward,
        down,
        up
    };
    struct step {
        direction dir;
        int magnitude;
    };
    using coordinates = std::tuple<int, int, int>;

    std::istream& operator>>(std::istream& in, step& out) {
        std::string line;
        int magnitude;
        getline(in, line);
        if (int a = sscanf(line.c_str(), "forward %d", &magnitude); a) {
            out = {direction::forward, magnitude};
        } else if (sscanf(line.c_str(), "down %d", &magnitude)) {
            out = {direction::down, magnitude};
        } else if (sscanf(line.c_str(), "up %d", &magnitude)) {
            out = {direction::up, magnitude};
        }
        return in;
    }

    void print_result(coordinates c) {
        auto [forward, depth, aim] = c;
        printf("Coordinates are %d forward and %d down\n", forward, depth);
        printf("Their product is %d\n", forward * depth);
    }

    coordinates move_version1(coordinates c, const step& d) {
        auto& [forward, depth, aim] = c;
        const auto& [dir, magnitude] = d;
        switch (dir) {
            case direction::forward:
                forward += magnitude;
                break;
            case direction::down:
                depth += magnitude;
                break;
            case direction::up:
                depth -= magnitude;
                break;
        }
        return c;
    }

    coordinates move_version2(coordinates c, const step& d) {
        auto& [forward, depth, aim] = c;
        const auto& [dir, length] = d;
        switch (dir) {
            case direction::forward:
                forward += length;
                depth += aim * length;
                break;
            case direction::down:
                aim += length;
                break;
            case direction::up:
                aim -= length;
                break;
        }
        return c;
    }

    template <std::ranges::range R>
    coordinates follow_path(R& vd, coordinates(*move_function)(coordinates, const step&)) {
        return std::accumulate(vd.begin(), vd.end(), coordinates(), move_function);
    }

    void puzzle1() {
        auto input_vector = GET_STREAM(input, step);
        auto result = follow_path(input_vector, move_version1);
        print_result(result);
    }

    void puzzle2() {
        auto input_vector = GET_STREAM(input, step);
        auto result = follow_path(input_vector, move_version2);

        print_result(result);
    }
}