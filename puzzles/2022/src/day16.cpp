//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/hash.h>
#include <unordered_set>
#include <set>
#include <numeric>
#include <algorithm>
#include <cstring>
#include <ox/graph.h>
#include <ox/grid.h>

namespace aoc2022::day16 {
    struct eruption {};
    using pipe_id_map = std::unordered_map<std::string, int>;
    using pipe_flow_list = std::vector<int>;
    using pipe_path_list = ox::grid<int>;

    using pipe_path_map = std::unordered_multimap<std::string, std::string>;
    using pipe_flow_map = std::unordered_map<std::string, int>;

    static pipe_path_map pipe_paths;
    static pipe_flow_map pipe_flows0;
    static pipe_path_list pipe_distances;
    static pipe_flow_list pipe_flows;
    static pipe_id_map valve_id_map;

    int get_pipe_index(const std::string& s) {
        if (auto x = valve_id_map.find(s); x != valve_id_map.end()) {
            return x->second;
        }
        auto x = valve_id_map.emplace(s, valve_id_map.size());
        return x.first->second;
    }

    struct vents : public std::array<char, 36> {
    private:
        vents& set_state(int vent, char state) {
            (*this)[vent] = state;
            return *this;
        }
        [[nodiscard]] bool is_state(int vent, char state) const { return (*this)[vent] == state; }
    public:
        [[nodiscard]] bool is_open(int vent) const { return is_state(vent, '*'); }
        [[nodiscard]] bool is_closed(int vent) const { return is_state(vent, '-'); }

        vents& open(int vent) { return set_state(vent, '*'); }
    };

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

    void calculate_path_distances(pipe_path_list& pipe_paths, const pipe_path_map& maps, const pipe_flow_map& flows) {
        int open_valves = 1 + static_cast<int>(stdr::distance(flows | stdv::values | stdv::filter(std::identity())));

        pipe_paths = pipe_path_list(open_valves, open_valves * open_valves);

        ox::dikstra_solver solver(std::string("AA"), always_false(), std::bind_front(get_neighbour_states, &maps));
        solver.set_debug_func([&](const std::string& end, const long& l, const auto&...) {
            if (flows.at(end))
                pipe_paths.at(get_pipe_index("AA"), get_pipe_index(end)) = static_cast<int>(l);
        });
        solver();

        for (auto& start : flows | stdv::filter(&pipe_flow_map::value_type::second) | stdv::keys) {
            ox::dikstra_solver solver(start, always_false(), std::bind_front(get_neighbour_states, &maps));
            solver.set_debug_func([&](const std::string& end, const long& l, const auto&...) {
                if (flows.at(end))
                    pipe_paths.at(get_pipe_index(start), get_pipe_index(end)) = static_cast<int>(l);
            });
            solver();
        }
    }

    void set_flows(pipe_flow_list& flows_dest, const pipe_flow_map& flows_src) {
        auto open_valves = 1 + stdr::distance(flows_src | stdv::values | stdv::filter(std::identity()));
        flows_dest.resize(open_valves);
        for (const auto& x : flows_src | stdv::filter(&pipe_flow_map::value_type::second)) {
            flows_dest[get_pipe_index(x.first)] = x.second;
        }
    }

    struct flow_state {
        vents pipes_opened;

        int time_remaining;
        int remaining_potential;
        int current_position;
        int elephant_position;
        int remaining_distance;

        void init() {
            remaining_distance = -1;
            std::memset(pipes_opened.data(), '-', 36);
            pipes_opened[pipe_flows.size()] = '\0';
            remaining_potential = std::accumulate(pipe_flows.begin(), pipe_flows.end(), 0);
        }

        [[nodiscard]] flow_state open_valve(int me) const {
            if (remaining_distance >= 0)
                return open_valve_with_elephant(me);

            auto distance = pipe_distances.at(current_position, me);
            if (distance >= time_remaining) {
                return stay();
            }
            auto new_state = *this;
            new_state.pipes_opened.open(me);
            new_state.remaining_potential -= pipe_flows[me];
            new_state.time_remaining -= distance + 1;
            new_state.current_position = me;
            return new_state;
        }

        [[nodiscard]] flow_state same_distance(int distance, int me, int elephant) const {
            auto new_state = *this;
            new_state.current_position = me;
            new_state.elephant_position = elephant;
            new_state.pipes_opened.open(me);
            new_state.pipes_opened.open(elephant);
            new_state.time_remaining -= distance + 1;
            new_state.remaining_potential -= pipe_flows[me] + pipe_flows[elephant];
            new_state.remaining_distance = -1;
            return new_state;
        }

        [[nodiscard]] flow_state update(int me, int elephant, int distance1, int distance2) const {
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

            new_state.pipes_opened.open(new_state.current_position);
            new_state.time_remaining -= distance1 + 1;
            new_state.remaining_potential -= pipe_flows[new_state.current_position];
            new_state.remaining_distance = (distance2 - distance1 - 1);

            return new_state;
        }

        [[nodiscard]] flow_state open_valve_with_elephant(int me) const {
            auto distance1 = pipe_distances.at(current_position, me);
            auto distance2 = remaining_distance;

            return update(me, elephant_position, distance1, distance2);
        }

        [[nodiscard]] flow_state open_valve(int me, int elephant) const {
            auto distance1 = pipe_distances.at(current_position, me);
            auto distance2 = pipe_distances.at(elephant_position, elephant);

            return update(me, elephant, distance1, distance2);
        }

        [[nodiscard]] flow_state open_last() const {
            auto distance1 = INT_MAX;
            auto distance2 = remaining_distance;

            return update(-1, elephant_position, distance1, distance2);
        }

        [[nodiscard]] flow_state stay() const {
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

        for (int new_position = 0; new_position < (int) pipe_flows.size(); ++new_position) {
            if (f.pipes_opened.is_open(new_position))
                continue;
            if (!pipe_flows[new_position])
                continue;
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

        auto destination_valves = stdv::iota(0, (int) pipe_flows.size())
                                | stdv::filter([&](int x) { return pipe_flows[x] && f.pipes_opened.is_closed(x); });

        if (f.remaining_distance < 0) {
            for (int a : destination_valves) {
                for (int b : destination_valves) {
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

        for (int new_position : destination_valves) {
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
        auto destination_valves = stdv::iota(0, (int) pipe_flows.size())
                                | stdv::filter([&](int x) { return pipe_flows[x] && f.pipes_opened.is_closed(x); })
                                | stdv::transform([&](int x) { return pipe_flows[x]; });

        std::vector remaining_valves(destination_valves.begin(), destination_valves.end());
        stdr::sort(remaining_valves, std::greater<>());

        int heurisitc = 0;
        auto head = remaining_valves.begin();
        while (time_remaining && potential) {
            heurisitc += potential;
            time_remaining--;
            potential -= *head;
            if (potential) {
                head++;
                potential -= *head;
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
            static_assert(std::is_trivial_v<aoc2022::day16::flow_state>);
            return std::hash<std::string_view>()(std::string_view(reinterpret_cast<const char*>(&x), sizeof(x)));
        };
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

    void solve(const char* filename, int start_time,
               std::vector<std::pair<flow_state, long>> (*neighbour_func)(const flow_state&)) {
        if (pipe_paths.empty()) {
            auto x = get_stream<ox::line>(filename);
            get_pipe_index("AA");
            stdr::for_each(x, std::bind_front(valve_data, std::ref(pipe_paths), std::ref(pipe_flows0)));
            calculate_path_distances(pipe_distances, pipe_paths, pipe_flows0);
            set_flows(pipe_flows, pipe_flows0);
        }

        flow_state start_state{
                .pipes_opened = {},
                .time_remaining = start_time,
                .remaining_potential = 0,
                .current_position = get_pipe_index("AA"),
                .elephant_position = get_pipe_index("AA"),
                .remaining_distance = -1
        };

        start_state.init();

        ox::dikstra_solver solver(ox::a_start{}, start_state, eruption(), neighbour_func, min_remaining_pressure_loss);
        solver.track_path();
        auto [path, cost] = solver();

        for (auto [x, y] : path) {
            printf("%2d & %2d: %s at t = %d -> %ld:\n",
                   x.current_position,
                   x.elephant_position,
                   x.pipes_opened.data() + 1,
                   start_time - x.time_remaining,
                   y);
        }
        printf("%ld\n", start_state.remaining_potential * (start_time) -cost);
    }

    void puzzle1(const char* filename) {
        solve(filename, 30, get_neighbour_states_alone);
    }

    void puzzle2(const char* filename) {
        solve(filename, 26, get_neighbour_states_with_elephant);
    }
} // namespace aoc2022::day16