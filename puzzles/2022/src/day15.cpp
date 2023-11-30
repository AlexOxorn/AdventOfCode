#include "../../../common.h"
#include <ox/grid.h>
#include <variant>
#include <stack>
#include <numeric>
#include <thread>
#include <ox/hash.h>
#include <ox/matrix.h>
#include <chrono>

namespace aoc2022::day15 {
    constexpr bool test = false;
    constexpr int depth = test ? 20 : 4'000'000;

    using coord = std::pair<long, long>;
    struct boundaries {
        long left, up, right, down;
        boundaries() = default;
        boundaries(long x, long y, long distance) :
                left(x - distance), up(y + distance), right(x + distance), down(y - distance) {}
    };

    long distance_between(coord start, coord end) {
        return std::abs(start.first - end.first) + std::abs(start.second - end.second);
    }

    coord rotate_ccl(int x, int y) {
        return {x - y, x + y};
    }
    coord rotate_cl(int x, int y) {
        return {(x + y)/2, (y - x)/2};
    }

    struct cave_map : public std::unordered_map<coord, int, ox::pair_hash<>> {
        using std::unordered_map<coord, int, ox::pair_hash<>>::unordered_map;

        void add_reading(const std::string& s) {
            int x1, x2, y1, y2;
            sscanf(s.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &x1, &y1, &x2, &y2);
            long distance = distance_between({x1, y1}, {x2, y2});
            insert(std::make_pair(coord{x1, y1}, distance));
            insert(std::make_pair(coord{x2, y2}, 0));
        }
    };

    boundaries covering_square(const std::string& s) {
        int x1, x2, y1, y2;
        sscanf(s.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d", &x1, &y1, &x2, &y2);
        long distance = distance_between({x1, y1}, {x2, y2});
        auto [new_x, new_y] = rotate_ccl(x1, y1);
        return {new_x, new_y, distance};
    }

    bool within_range(coord position, cave_map::value_type sensor) {
        auto& [end, distance] = sensor;
        return distance_between(position, end) <= distance;
    }

    std::vector<int> get_candiates(const std::vector<int>& low, const std::vector<int>& high) {
        auto low_begin = low.begin();
        auto low_end = low.end();
        auto high_begin = high.begin();
        auto high_end = high.end();

        std::vector<int> to_return;

        while (low_begin != low_end && high_begin != high_end) {
            if (*low_begin < *high_begin + 2) {
                ++low_begin;
                continue;
            }

            if (*low_begin - *high_begin == 2) {
                to_return.push_back(*high_begin + 1);
            }
            ++high_begin;
        }

        return to_return;
    }

    cave_map& get_cave_map(puzzle_options filename) {
        static std::optional<cave_map> map;
        if (map.has_value())
            return *map;

        map.emplace();
        auto input = get_stream<ox::line>(filename);
        stdr::for_each(input, [](const auto& x) { map->add_reading(x); });
        return *map;
    }

    answertype puzzle1(puzzle_options filename) {
        cave_map& map = get_cave_map(filename);
        auto left_boundary = stdr::min(map | stdv::transform([](const auto& r) { return r.first.first - r.second; }));
        auto right_boundary = stdr::max(map | stdv::transform([](const auto& r) { return r.first.first + r.second; }));

        auto visible_spots =
                stdv::iota(left_boundary, right_boundary) | stdv::transform([](int x) { return coord(x, depth / 2); })
                | stdv::filter([&map](coord c) { return !map.contains(c) || map[c] != 0; })
                | stdv::filter([&map](coord c) { return stdr::any_of(map, std::bind_front(within_range, c)); });
        std::size_t count = stdr::distance(visible_spots);
        myprintf("There are %zu spots where the distress signal can't be\n", count);
        return count;
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<ox::line>(filename);
        auto boundary_stream = input | stdv::transform(&covering_square);
        std::vector<int> upper_boundaries;
        std::vector<int> lower_boundaries;
        std::vector<int> left_boundaries;
        std::vector<int> right_boundaries;

        for (auto [l, u, r, d] : boundary_stream) {
            upper_boundaries.push_back(u);
            lower_boundaries.push_back(d);
            left_boundaries.push_back(l);
            right_boundaries.push_back(r);
        }

        stdr::sort(upper_boundaries);
        stdr::sort(lower_boundaries);
        stdr::sort(left_boundaries);
        stdr::sort(right_boundaries);

        auto vertical_boundaries = get_candiates(lower_boundaries, upper_boundaries);
        auto horizontal_boundaries = get_candiates(left_boundaries, right_boundaries);

        cave_map& map = get_cave_map(filename);

        for (int old_x : horizontal_boundaries) {
            for (int old_y : vertical_boundaries) {
                auto candidate_pos = rotate_cl(old_x, old_y);
                if (stdr::all_of(map, std::not_fn(std::bind_front(within_range, candidate_pos)))) {
                    long result = candidate_pos.first * 4'000'000 + candidate_pos.second;
                    myprintf("%ld\n", result);
                    return result;
                }
            }
        }
        __builtin_unreachable();
    }
} // namespace aoc2022::day15