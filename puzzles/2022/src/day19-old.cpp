#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <numeric>

namespace aoc2022::day19 {
    constexpr int rock_types = 4;
    struct material {
        std::array<long, rock_types> rocks;

        bool operator<=(const material& other) const {
            for (int i = 0; i < rock_types; i++)
                if (rocks[i] > other.rocks[i])
                    return false;
            return true;
        }

        bool operator==(const material&) const = default;

        auto operator<=>(const material& other) const {
            if (*this == other)
                return std::partial_ordering::equivalent;
            if (*this <= other)
                return std::partial_ordering::less;
            return std::partial_ordering::unordered;
        }

        material& operator-=(const material& other) {
            for (int i = 0; i < rock_types; i++)
                rocks[i] -= other.rocks[i];
            return *this;
        }

        [[nodiscard]] bool savable(std::array<long, rock_types> robots) const {
            for (int i = 0; i < rock_types; i++) {
                if (rocks[i] && robots[i])
                    return false;
            }
            return true;
        }
    };

    struct blueprint {
        int id;
        material ore_robot, clay_robot, obsidian_robot, geode_robot;
        std::array<long, rock_types> max_robots;

        [[nodiscard]] size_t savable(std::array<long, rock_types> robots) const {
            return stdr::count_if(std::array{ore_robot, clay_robot, obsidian_robot, geode_robot},
                                  [&](material& m) { return m.savable(robots); });
        }

        void set_maximums() {
            auto x = std::array{ore_robot, clay_robot, obsidian_robot, geode_robot};
            for (int i = 0; i < rock_types - 1; i++) {
                max_robots[i] = stdr::max(x, std::less<>(), [i](material m) { return m.rocks[i]; }).rocks[i];
            }
            max_robots[rock_types - 1] = std::numeric_limits<long>::max();
        }
    };

    static material blueprint::*robots[rock_types] = {
            &blueprint::ore_robot, &blueprint::clay_robot, &blueprint::obsidian_robot, &blueprint::geode_robot};

    struct state {
        const blueprint* blue;
        std::array<long, rock_types> robots;
        material curren_material;

        [[nodiscard]] state do_nothing() const {
            auto to_return = *this;
            for (int i = 0; i < rock_types; ++i) {
                to_return.curren_material.rocks[i] += robots[i];
            }
            return to_return;
        }

        [[nodiscard]] state buy(material blueprint::*mat, int index) const {
            auto to_return = *this;
            ++to_return.robots[index];
            to_return.curren_material -= blue->*mat;
            return to_return;
        }

        [[nodiscard]] size_t savable() const { return blue->savable(robots); }
    };

    std::vector<state> get_neighbours(const state& s) {
        std::vector<state> neighbours;
        neighbours.reserve(rock_types);

        auto new_state = s.do_nothing();
        for (int i = 0; i < rock_types; i++) {
            if (s.robots[i] < s.blue->max_robots[i] && s.blue->*(robots[i]) <= s.curren_material) {
                neighbours.push_back(new_state.buy(robots[i], i));
            }
        }
        neighbours.push_back(new_state);
        return neighbours;
    }

    long score(std::array<long, rock_types> a) {
        return (a[3] << 48) + (a[2] << 32) + (a[1] << 16) + a[0];
    }

    std::vector<state> prune(const std::vector<state>& states) {
        std::unordered_map<long, std::vector<std::pair<state, material>>> max_scores;

        for (const state& s : states) {
            long robots_key = score(s.robots);
            if (!max_scores.contains(robots_key)) {
                max_scores.emplace(robots_key, std::vector{std::make_pair(s, s.curren_material)});
                continue;
            }

            bool found = false;
            for (auto& state : max_scores.at(robots_key)) {
                if (state.second <=> s.curren_material == std::partial_ordering::less) {
                    found = true;
                    state = std::make_pair(s, s.curren_material);
                }
            }
            if (not found) {
                max_scores.at(robots_key).emplace_back(s, s.curren_material);
            }
        }
        myprintf("=====================================\n");
        auto x = max_scores | stdv::values | stdv::join | stdv::keys;
        return {x.begin(), x.end()};
    }

    std::vector<state> calculate_next(const std::vector<state>& in, [[maybe_unused]] int remaining_time = 1) {
        std::vector<state> nexts;
        nexts.reserve(in.size() * rock_types);

        for (const auto& x : in) {
            auto v = get_neighbours(x);
            nexts.insert(nexts.end(), std::make_move_iterator(v.begin()), std::make_move_iterator(v.end()));
        }

        myprintf("%zu\n", nexts.size());
        return prune(nexts);
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

        for (auto robot : robots) {
            char mineral_type[10];
            sscanf(head, "Each %s robot costs %n", mineral_type, &read_distance);
            head += read_distance;

            (b.*robot) = {};

            while (true) {
                int amount;
                sscanf(head, "%d %[^ .]%n", &amount, mineral_type, &read_distance);
                head += read_distance;
                if ("ore"sv == mineral_type) {
                    (b.*robot).rocks[0] = amount;
                } else if ("clay"sv == mineral_type) {
                    (b.*robot).rocks[1] = amount;
                } else if ("obsidian"sv == mineral_type) {
                    (b.*robot).rocks[2] = amount;
                } else if ("geode"sv == mineral_type) {
                    (b.*robot).rocks[3] = amount;
                }

                if (*head == '.')
                    break;
                head += 5;
            }
            head += 2;
        }

        return in;
    }

    state geode_count(const blueprint& b) {
        std::vector start{
                state{.blue = &b, .robots = {1, 0, 0, 0}, .curren_material = {}}
        };

        for (int i = 0; i < 24; i++) {
            start = calculate_next(start, 24 - i);
        }

        auto result =
                stdr::max(start, std::less<>(), [](const state& x) { return x.curren_material.rocks[rock_types - 1]; });
        myprintf("state:\n\tID: %d\n\tore: %ld\n\tclay: %ld\n\tobsidian: %ld\n\tgeodes: %ld\n",
               result.blue->id,
               result.curren_material.rocks[0],
               result.curren_material.rocks[1],
               result.curren_material.rocks[2],
               result.curren_material.rocks[3]);
        return result;
    }

    answertype puzzle1(puzzle_options filename) {
        auto blueprints = get_from_input<blueprint>(filename);
        std::vector<state> results;
        stdr::for_each(blueprints, &blueprint::set_maximums);
        stdr::transform(blueprints, std::back_inserter(results), geode_count);
        //        auto best = stdr::max_element(results, std::less<>(), [](const state& s) { return
        //        s.curren_material.geode; });
        auto quality_values = results | stdv::transform([](const state& s) {
                                  myprintf("%d - %ld\n", s.blue->id, s.curren_material.rocks[rock_types - 1]);
                                  return s.blue->id * s.curren_material.rocks[rock_types - 1];
                              });
        myprintf("Sum of Quality Level %ld\n", std::accumulate(quality_values.begin(), quality_values.end(), 0l));
        return {};
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) { return {}; }
} // namespace aoc2022::day19