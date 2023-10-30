//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/hash.h>
#include <chrono>
#include <unordered_set>
#include <set>
#include <numeric>
#include <algorithm>
#include <ox/graph.h>

namespace aoc2022::day16 {
    struct eruption {};
    using pipe_path_map = std::unordered_multimap<std::string, std::string>;
    using pipe_flow_map = std::unordered_map<std::string, int>;
    using pipe_open_set = std::unordered_set<std::string>;
    using path_distance = std::unordered_map<std::string, int>;

    auto get_neighbour_states(const pipe_path_map* pipe_paths, const std::string& node) {
        std::vector<std::pair<std::string, int>> to_return;
        auto [start, end] = pipe_paths->equal_range(node);
        for (const auto& x : stdr::subrange(start, end) | stdv::values) {
            to_return.emplace_back(x, 1);
        }
        return to_return;
    }

    struct always_false {};

    bool operator==(const std::string&, const always_false&) {
        return false;
    }

    std::string path_key(const std::string& start, const std::string& end) {
        std::string res;
        res += start;
        res += "->";
        res += end;
        return res;
    }

    void calculate_path_distances(path_distance& pipe_paths, const pipe_path_map& maps, const pipe_flow_map& flows) {
        ox::dikstra_solver solver(std::string("AA"), always_false(), std::bind_front(get_neighbour_states, &maps));
        solver.set_debug_func([&](const std::string& end, const long& l, const auto&...) {
            if (flows.at(end))
                pipe_paths.insert(std::make_pair(path_key("AA", end), l));
        });
        solver();

        for (auto& start : flows | stdv::filter(&pipe_flow_map::value_type::second) | stdv::keys) {
            ox::dikstra_solver solver(start, always_false(), std::bind_front(get_neighbour_states, &maps));
            solver.set_debug_func([&](const std::string& end, const long& l, const auto&...) {
                if (flows.at(end))
                    pipe_paths.insert(std::make_pair(path_key(start, end), l));
            });
            solver();
        }
    }

    struct flow_state {
        const pipe_path_map* pipe_paths;
        const pipe_flow_map* pipe_flows;
        const path_distance* path_distances;
        pipe_open_set pipes_opened = {};

        int time_remaining = 30;
        int remaining_potential = -1;

        std::string current_position;
        std::string elephant_position;

        int remaining_distance = -1;

        void set_initial_potential() {
            auto x = *(pipe_flows) | stdv::values;
            remaining_potential = std::accumulate(x.begin(), x.end(), 0);
        }

        flow_state open_valve(const std::string& me) const {
            if (remaining_distance >= 0)
                return open_valve_with_elephant(me);

            auto distance = path_distances->at(path_key(current_position, me));
            if (distance >= time_remaining) {
                return stay();
            }
            auto new_state = *this;
            new_state.pipes_opened.emplace(me);
            new_state.remaining_potential -= pipe_flows->at(me);
            new_state.time_remaining -= distance + 1;
            new_state.current_position = me;
            return new_state;
        }

        flow_state same_distance(int distance, const std::string& me, const std::string& elephant) const {
            auto new_state = *this;
            new_state.current_position = me;
            new_state.elephant_position = elephant;
            new_state.pipes_opened.emplace(me);
            new_state.pipes_opened.emplace(elephant);
            new_state.time_remaining -= distance + 1;
            new_state.remaining_potential -= pipe_flows->at(me) + pipe_flows->at(elephant);
            new_state.remaining_distance = -1;
            return new_state;
        }

        flow_state update(const std::string& me, const std::string& elephant, int distance1, int distance2) const {
            if (std::min(distance1, distance2) >= time_remaining) {
                return stay();
            }

            if (distance1 == distance2) {
                return same_distance(distance1, me, elephant);
            }

            auto new_state = *this;
            new_state.current_position = me;
            new_state.elephant_position = elephant;

            if (distance1 > distance2) {
                std::swap(distance1, distance2);
                std::swap(new_state.current_position, new_state.elephant_position);
            }

            new_state.pipes_opened.emplace(new_state.current_position);
            new_state.time_remaining -= distance1 + 1;
            new_state.remaining_potential -= pipe_flows->at(new_state.current_position);
            new_state.remaining_distance = (distance2 - distance1 - 1);

            return new_state;
        }

        flow_state open_valve_with_elephant(const std::string& me) const {
            auto distance1 = path_distances->at(path_key(current_position, me));
            auto distance2 = remaining_distance;

            return update(me, elephant_position, distance1, distance2);
        }

        flow_state open_valve(const std::string& me, const std::string& elephant) const {
            auto distance1 = path_distances->at(path_key(current_position, me));
            auto distance2 = path_distances->at(path_key(elephant_position, elephant));

            return update(me, elephant, distance1, distance2);
        }

        flow_state open_last() const {
            auto distance1 = INT_MAX;
            auto distance2 = remaining_distance;

            return update("", elephant_position, distance1, distance2);
        }

        flow_state stay() const {
            auto new_state = *this;
            new_state.time_remaining = 0;
            return new_state;
        }

        bool operator==(const flow_state&) const = default;
    };

    bool operator==(const flow_state& f, const eruption&) {
        return f.time_remaining == 0;
    }

    auto get_neighbour_states_alone(const flow_state& f) {
        std::vector<std::pair<flow_state, long>> to_return;
        if (f.time_remaining == 0)
            return to_return;

        if (f.remaining_potential == 0) {
            flow_state new_state = f.stay();
            return std::vector{std::make_pair(new_state, 0l)};
        }

        for (const std::string& new_position : *(f.pipe_flows) | stdv::filter([&](auto& x) {
                 return x.second && !f.pipes_opened.contains(x.first);
             }) | stdv::keys) {
            if (new_position == f.current_position)
                continue;
            auto new_state = f.open_valve(new_position);
            auto cost = (f.time_remaining - new_state.time_remaining) * f.remaining_potential;
            to_return.emplace_back(new_state, cost);
        }

        return to_return;
    }

    auto get_neighbour_states_with_elephant(const flow_state& f) {
        std::vector<std::pair<flow_state, long>> to_return;
        if (f.time_remaining == 0)
            return to_return;

        if (f.remaining_potential == 0) {
            flow_state new_state = f.stay();
            return std::vector{std::make_pair(new_state, 0l)};
        }

        auto destination_valves = *(f.pipe_flows)
                                | stdv::filter([&](auto& x) { return x.second && !f.pipes_opened.contains(x.first); })
                                | stdv::keys;

        if (f.remaining_distance < 0) {
            for (const auto& a : destination_valves) {
                for (const auto& b : destination_valves) {
                    if (a == b)
                        continue;
                    if (f.current_position == a || f.elephant_position == a)
                        continue;
                    if (f.current_position == b || f.elephant_position == b)
                        continue;

                    auto new_state = f.open_valve(a, b);
                    auto cost = (f.time_remaining - new_state.time_remaining) * f.remaining_potential;
                    to_return.emplace_back(new_state, cost);
                }
            }
            return to_return;
        }

        for (const std::string& new_position : destination_valves) {
            if (new_position == f.current_position || new_position == f.elephant_position)
                continue;
            auto new_state = f.open_valve(new_position);
            auto cost = (f.time_remaining - new_state.time_remaining) * f.remaining_potential;
            to_return.emplace_back(new_state, cost);
        }

        if (to_return.empty()) {
            auto new_state = f.open_last();
            auto cost = (f.time_remaining - new_state.time_remaining) * f.remaining_potential;
            to_return.emplace_back(new_state, cost);
        }

        return to_return;
    }

    auto min_remaining_pressure_loss(const flow_state& f, eruption) {
        int time_remaining = f.time_remaining;
        int potential = f.remaining_potential;
        auto destination_valves = *(f.pipe_flows)
                                | stdv::filter([&](auto& x) { return x.second && !f.pipes_opened.contains(x.first); })
                                | stdv::values;
        std::vector remaining_valves(destination_valves.begin(), destination_valves.end());
        stdr::sort(remaining_valves, std::greater<>());

        int heurisitc = 0;
        auto head = remaining_valves.begin();
        while (time_remaining && potential) {
            heurisitc += potential;
            time_remaining--;
            potential -= head[0];
            if (potential) {
                head++;
                potential -= head[0];
                head++;
            }
        }

        return heurisitc;
    }

} // namespace aoc2022::day16

namespace std {
    template <>
    struct hash<aoc2022::day16::flow_state> {
        size_t operator()(const aoc2022::day16::flow_state& x) const {
            size_t result = 0;
            for (auto& open_pipe : x.pipes_opened) {
                result += std::hash<std::string>()(open_pipe);
            }
            result += std::hash<std::string>()(x.current_position);
            result += std::hash<std::string>()(x.elephant_position);
            result += std::hash<int>()(x.remaining_potential);
            result += std::hash<int>()(x.remaining_distance);
            result += std::hash<int>()(x.time_remaining);
            return result;
        }
    };
} // namespace std

namespace aoc2022::day16 {
    void valve_data(pipe_path_map& pipe_paths, pipe_flow_map& pipe_flows, const std::string& s) {
        char source[3], dest[3];
        int flow, read_upto;
        if (3 != sscanf(s.c_str(), "Valve %02s has flow rate=%d; tunnels lead to valves %n", source, &flow, &read_upto))
            sscanf(s.c_str(), "Valve %02s has flow rate=%d; tunnel leads to valve %n", source, &flow, &read_upto);
        pipe_flows.insert({source, flow});
        const char* head = s.c_str() + read_upto;
        while (head < s.end().base()) {
            sscanf(head, "%02s", dest);
            pipe_paths.insert({source, dest});
            head += 2;
            if (*head == ',')
                head += 2;
        }
    }

    auto solve(const char* filename, int start_time,
               std::vector<std::pair<flow_state, long>> (*neighbour_func)(const flow_state&)) {
        static pipe_path_map pipe_paths;
        static pipe_flow_map pipe_flows;
        static path_distance pipe_distances;

        if (pipe_paths.empty()) {
            auto x = get_stream<ox::line>(filename);
            stdr::for_each(x, std::bind_front(valve_data, std::ref(pipe_paths), std::ref(pipe_flows)));
            calculate_path_distances(pipe_distances, pipe_paths, pipe_flows);
        }

        path_distance d = pipe_distances;

        flow_state start_state{.pipe_paths = &pipe_paths,
                               .pipe_flows = &pipe_flows,
                               .path_distances = &pipe_distances,
                               .time_remaining = start_time,
                               .current_position = "AA",
                               .elephant_position = "AA"};

        start_state.set_initial_potential();

        ox::dikstra_solver solver(ox::a_start{}, start_state, eruption(), neighbour_func, min_remaining_pressure_loss);

        auto [path, cost] = solver();

        for (auto [x, y] : path) {
            myprintf("%s & %s at t = %d -> %ld\n",
                   x.current_position.c_str(),
                   x.elephant_position.c_str(),
                   start_time - x.time_remaining,
                   y);
        }
        long final_cost = start_state.remaining_potential * (start_time) -cost;
        myprintf("%ld\n", final_cost);
        return final_cost;
    }

    answertype puzzle1(const char* filename) {
        return solve(filename, 30, get_neighbour_states_alone);
    }

    answertype puzzle2(const char* filename) {
        return solve(filename, 26, get_neighbour_states_with_elephant);
    }
} // namespace aoc2022::day16