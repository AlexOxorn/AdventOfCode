//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <numeric>
#include <ox/grid.h>
#include <ox/graph.h>

namespace aoc2022::day24 {
    constexpr char empty = 0;
    constexpr char up = 1 << 0;
    constexpr char down = 1 << 1;
    constexpr char left = 1 << 2;
    constexpr char right = 1 << 3;
    constexpr char wall = 1 << 4;

    char bitmap(char c) {
        switch (c) {
            case '#': return wall;
            case '.': return empty;
            case '<': return left;
            case '>': return right;
            case 'v': return down;
            case '^': return up;
            default: throw std::exception();
        }
    }

    auto& get_blizzards(const char* filename = "") {
        static std::vector<ox::grid<char>> blizzard{ox::grid<char>(get_stream<ox::line>(filename), bitmap)};
        return blizzard;
    }

    ox::grid<char> next_blizzard(const ox::grid<char>& blizzard) {
        ox::grid<char> next(static_cast<int>(blizzard.get_width()), blizzard.get_size());

        for (int y = 0; y < static_cast<int>(blizzard.get_height()); y++) {
            for (int x = 0; x < static_cast<int>(blizzard.get_width()); x++) {
                auto& data = blizzard.at(x, y);
                if (data == wall) {
                    next.at(x, y) = wall;
                    continue;
                }
                if (data & up) {
                    int new_y = y <= 1 ? static_cast<int>(blizzard.get_height()) - 2 : y - 1;
                    next.at(x, new_y) |= up;
                }
                if (data & down) {
                    int new_y = y >= static_cast<int>(blizzard.get_height()) - 2 ? 1 : y + 1;
                    next.at(x, new_y) |= down;
                }
                if (data & left) {
                    int new_x = x <= 1 ? static_cast<int>(blizzard.get_width()) - 2 : x - 1;
                    next.at(new_x, y) |= left;
                }
                if (data & right) {
                    int new_x = x >= static_cast<int>(blizzard.get_width()) - 2 ? 1 : x + 1;
                    next.at(new_x, y) |= right;
                }
            }
        }

        return next;
    }

    const ox::grid<char>& blizzard_at_time(size_t t) {
        auto& x = get_blizzards();
        while (x.size() <= t)
            x.push_back(next_blizzard(x.back()));
        return x[t];
    }

    struct end_state {
        int x, y, lap, start_x, start_y;
    };

    struct state {
        int x, y, time, lap;

        bool operator==(const state&) const = default;

        [[nodiscard]] state creat_new_state(int x, int y) const {
            const auto& blizzard = blizzard_at_time(time + 1);

            auto s = *this;
            s.x += x;
            s.y += y;
            s.time++;
            if (s.y == static_cast<int>(blizzard.get_height()) - 1 && (lap % 2) == 0) {
                s.lap++;
            } else if (s.y == 0 && (lap % 2) == 1) {
                s.lap++;
            }
            return s;
        }

        [[nodiscard]] std::vector<std::pair<state, int>> get_neighbours() const {
            const auto& blizzard = blizzard_at_time(time + 1);
            std::vector<std::pair<state, int>> neighbours;
            if (blizzard.at(x, y) == empty) {
                neighbours.emplace_back(creat_new_state(0, 0), 1);
            }
            if (y > 0 && blizzard.at(x, y - 1) == empty) {
                neighbours.emplace_back(creat_new_state(0, -1), 1);
            }
            if (y < static_cast<int>(blizzard.get_height()) - 1 && blizzard.at(x, y + 1) == empty) {
                neighbours.emplace_back(creat_new_state(0, 1), 1);
            }
            if (x > 0 && blizzard.at(x - 1, y) == empty) {
                neighbours.emplace_back(creat_new_state(-1, 0), 1);
            }
            if (x < static_cast<int>(blizzard.get_width()) - 1 && blizzard.at(x + 1, y) == empty) {
                neighbours.emplace_back(creat_new_state(1, 0), 1);
            }
            return neighbours;
        }

        [[nodiscard]] int heuristic(end_state end) const {
            int rest_base = lap % 2 ? std::abs(end.start_x - x) + std::abs(end.start_y - y)
                                    : std::abs(end.x - x) + std::abs(end.y - y);
            return rest_base + (end.lap - lap - 1) * (std::abs(end.start_x - end.x) + std::abs(end.start_y - end.y));
        }
    };

    bool operator==(const state& s, const end_state& e) {
        return s.x == e.x && s.y == e.y && s.lap == e.lap;
    }
} // namespace aoc2022::day24

namespace std {
    template <>
    struct hash<aoc2022::day24::state> {
        size_t operator()(const aoc2022::day24::state& s) const {
            auto x = std::string_view(reinterpret_cast<const char*>(&s), sizeof(s));
            return std::hash<std::string_view>()(x);
        }
    };
} // namespace std

namespace aoc2022::day24 {
    std::tuple<int, int, int, int> start_and_end(const char* filename) {
        auto blizzard = get_blizzards(filename).front();
        auto start_itr = stdr::find(blizzard.get_raw(), empty);
        auto end_itr = stdr::find(blizzard.get_raw() | stdv::reverse, empty).base();

        auto [start_x, start_y] = blizzard.coord_from_index(start_itr);
        auto [end_x, end_y] = blizzard.coord_from_index(end_itr - 1);

        return {start_x, start_y, end_x, end_y};
    }

    void puzzle1(const char* filename) {
        auto [start_x, start_y, end_x, end_y] = start_and_end(filename);

        ox::dikstra_solver dd(ox::a_start(),
                              state{start_x, start_y, 0, 0},
                              end_state{end_x, end_y, 1, start_x, start_y},
                              &state::get_neighbours,
                              &state::heuristic);

        auto [path, cost] = dd();
        printf("the time it takes to cross the blizzard is %d\n", cost);
    }

    void puzzle2(const char* filename) {
        auto [start_x, start_y, end_x, end_y] = start_and_end(filename);

        ox::dikstra_solver dd(ox::a_start(),
                              state{start_x, start_y, 0, 0},
                              end_state{end_x, end_y, 3, start_x, start_y},
                              &state::get_neighbours,
                              &state::heuristic);

        auto [path, cost] = dd();
        printf("the time it takes to cross the blizzard is %d\n", cost);
    }
} // namespace aoc2022::day24