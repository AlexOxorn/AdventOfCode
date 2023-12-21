#include "../../../common.h"
#include <charconv>
#include <format>
#include <ox/formatting.h>
#include <ox/colors.h>
#include <ox/polygon.h>

namespace aoc2023::day18 {
    struct dig_directions {
        uint32_t color;
        uint32_t amount;
        char dir;
        char dir2;
    };

    // 0 means R, 1 means D, 2 means L, and 3 means U.
    char to_dir(char c) {
        switch (c) {
            case '0': return 'R';
            case '1': return 'D';
            case '2': return 'L';
            case '3': return 'U';
            default: std::unreachable();
        }
    }

    STREAM_IN(dig_directions, d) {
        std::string color;
        in >> d.dir >> d.amount >> color;
        if (!in)
            return in;
        std::from_chars(color.c_str() + 2, color.c_str() + 7, d.color, 16);
        d.dir2 = to_dir(color[7]);
        return in;
    }

    STREAM_OUT(dig_directions, d) {
        return out << ox::color(d.color) << d.dir << " " << d.amount << ox::format{ox::escape::reset} << std::endl;
    }

    std::array<long, 2> move(std::array<long, 2> pos, char dir, long distance = 1) {
        switch (dir) {
            case 'U': return {pos[0], pos[1] - distance};
            case 'L': return {pos[0] - distance, pos[1]};
            case 'D': return {pos[0], pos[1] + distance};
            case 'R': return {pos[0] + distance, pos[1]};
            default: std::unreachable();
        }
    }

    answertype solve(puzzle_options filename, char dig_directions::* direction, uint32_t dig_directions::* amount) {
        auto input = get_from_input<dig_directions>(filename);
        ox::polygon<long> p;
        std::array pos{0l, 0l};
        for (auto inst : input) {
            p.points.emplace_back(pos[0], pos[1]);
            pos = move(pos, inst.*direction, inst.*amount);
        }

        long res2 = long(p.area()) + long(p.perimeter()) / 2 + 1;
        printf("%ld\n", res2);

        return res2;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        return solve(filename, &dig_directions::dir, &dig_directions::amount);
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        return solve(filename, &dig_directions::dir2, &dig_directions::color);
    }
} // namespace aoc2023::day18