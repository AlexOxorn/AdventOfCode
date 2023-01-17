//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <numeric>
#include <ox/types.h>
#include <ox/graph.h>

namespace aoc2022::day19 {
#define FOR_ROCK for (int i = 0; i < rock_types; i++)
    using namespace ox::int_alias;

    constexpr int rock_types = 4;

    u16 triangle_sum2(u16 steps, u16 robots, u16 time_remaining) {
        u16 res = 0;
        for (int i = 0; i < time_remaining; ++i) {
            res += steps - (robots + i);
        }
        return res;
    }

    union material {
        u16 rocks[rock_types];
        i64 data;
        struct {
            u16 ore, clay, obsidian, geode;
        };

        material& operator-=(const material& other) {
            FOR_ROCK {
                rocks[i] -= other.rocks[i];
            }
            return *this;
        }

        material& operator+=(const material& other) {
            FOR_ROCK {
                rocks[i] += other.rocks[i];
            }
            return *this;
        }
        material operator+(const material& other) const {
            auto res = *this;
            res += other;
            return res;
        }

        bool operator==(const material& other) const { return this->data == other.data; };
        auto operator<=>(const material& other) const { return this->data <=> other.data; };

        [[nodiscard]] bool can_by(material m) const {
            FOR_ROCK {
                if (rocks[i] < m.rocks[i])
                    return false;
            }
            return true;
        }
    };

    struct blueprint {
        int id;
        material ore_robot, clay_robot, obsidian_robot, geode_robot;
    };

    static material blueprint::*robot_blueprints[rock_types] = {
            &blueprint::ore_robot, &blueprint::clay_robot, &blueprint::obsidian_robot, &blueprint::geode_robot};

    struct end_state {};

    struct state {
        const blueprint* blue;
        material robots;
        material curren_material;
        u8 time_state; u8 steps;

        bool operator==(const state&) const = default;

        [[nodiscard]] state do_nothing() const {
            auto to_return = *this;
            for (int i = 0; i < rock_types; ++i) {
                to_return.curren_material.rocks[i] += robots.rocks[i];
            }
            to_return.time_state++;
            return to_return;
        }

        [[nodiscard]] state buy(material blueprint::*mat, int index) const {
            auto to_return = *this;
            ++to_return.robots.rocks[index];
            to_return.curren_material -= blue->*mat;
            return to_return;
        }

        material cost() {
            material cost{.rocks = {}};
            FOR_ROCK {
                cost.rocks[i] = steps - robots.rocks[i];
            }
            return cost;
        }

        [[nodiscard]] std::vector<std::pair<state, material>> get_neighbours() const {
            std::vector<std::pair<state, material>> neighbours;
            neighbours.reserve(rock_types + 1);

            auto new_state = do_nothing();
            FOR_ROCK {
                if (curren_material.can_by(blue->*(robot_blueprints[i]))) {
                    auto buy_state = new_state.buy(robot_blueprints[i], i);
                    neighbours.emplace_back(buy_state, buy_state.cost());
                }
            }
            if (neighbours.size() != 4)
                neighbours.emplace_back(new_state, new_state.cost());
            return neighbours;
        }

        material heuristic(...) const {
            u16 remaining = steps - time_state;
            return material{.geode = triangle_sum2(steps, robots.geode, remaining),
                            .ore = triangle_sum2(steps, robots.ore, remaining),
                            .obsidian = triangle_sum2(steps, robots.obsidian, remaining),
                            .clay = triangle_sum2(steps, robots.clay, remaining)};
        }
    };
} // namespace aoc2022::day19

namespace std {
    template <>
    struct std::hash<aoc2022::day19::state> {
        size_t operator()(const aoc2022::day19::state& s) const {
            auto sv = std::string_view(reinterpret_cast<const char*>(&s), sizeof(s));
            return std::hash<std::string_view>()(sv);
        }
    };
} // namespace std

namespace aoc2022::day19 {
    bool operator==(const state& s, const end_state&) {
        return s.time_state == s.steps;
    }

    std::istream& operator>>(std::istream& in, blueprint& b) {
        using namespace std::literals::string_view_literals;

        std::string line;
        std::getline(in, line);
        if (!in)
            return in;

        const char* head = line.begin().base();
        int read_distance;

        sscanf(head, "Blueprint %d: %n", &b.id, &read_distance);
        head += read_distance;

        for (auto robot : robot_blueprints) {
            char mineral_type[10];
            sscanf(head, "Each %s robot costs %n", mineral_type, &read_distance);
            head += read_distance;

            (b.*robot) = {};

            while (true) {
                int amount;
                sscanf(head, "%d %[^ .]%n", &amount, mineral_type, &read_distance);
                head += read_distance;
                if ("ore"sv == mineral_type) {
                    (b.*robot).ore = amount;
                } else if ("clay"sv == mineral_type) {
                    (b.*robot).clay = amount;
                } else if ("obsidian"sv == mineral_type) {
                    (b.*robot).obsidian = amount;
                } else if ("geode"sv == mineral_type) {
                    (b.*robot).geode = amount;
                }

                if (*head == '.')
                    break;
                head += 5;
            }
            head += 2;
        }

        return in;
    }

    std::pair<i32, u16> geode_count(const blueprint& b, u8 steps) {
        material start{.ore = 1, .clay = 0, .obsidian = 0, .geode = 0};

        ox::dikstra_solver solver(
                ox::a_start(),
                state{.blue = &b, .robots = start, .curren_material = {}, .time_state = 0, .steps = steps},
                end_state(),
                &state::get_neighbours,
                &state::heuristic);

        auto [path, cost] = solver();

        auto x = path.back();

        return {b.id, x.first.curren_material.geode};
    }

    void puzzle1(const char* filename) {
        auto blueprints = get_from_input<blueprint>(filename);
        auto quality_scores = blueprints | stdv::transform([](const auto& x) { return geode_count(x, 24); })
                            | stdv::transform([](const std::pair<i32, u16>& a) { return a.first * a.second; });
        auto result = std::accumulate(quality_scores.begin(), quality_scores.end(), 0);

        printf("The total quality score is %d\n", result);
    }

    void puzzle2(const char* filename) {
        auto blueprints = get_from_input<blueprint>(filename);
        auto quality_scores = blueprints | stdv::take(3)
                            | stdv::transform([](const auto& x) { return geode_count(x, 32); })
                            | stdv::transform([](std::pair<i32, u16> a) { return a.second; });

        auto result = std::accumulate(quality_scores.begin(), quality_scores.end(), 1, std::multiplies());

        printf("The total geode product is %d\n", result);
    }
} // namespace aoc2022::day19