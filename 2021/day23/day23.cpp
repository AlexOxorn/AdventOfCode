//
// Created by alexoxorn on 2021-12-27.
//

#include "day23.h"
#include <set>
#include <map>
#include <numeric>
#include <ox/graph.h>
#include <ox/grid.h>

#define YEAR 2021
#define DAY 23

namespace day23 {
    template<size_t Depth>
    struct cave_state {
        using cave = std::array<char, Depth>;
        std::array<char, 11> hallway{};
        std::array<cave, 4> caves;

        cave_state(std::initializer_list<char> a, std::initializer_list<char> b, std::initializer_list<char> c, std::initializer_list<char> d) {
            stdr::copy(a, caves[0].begin());
            stdr::copy(b, caves[1].begin());
            stdr::copy(c, caves[2].begin());
            stdr::copy(d, caves[3].begin());
        }
        bool operator<=>(const cave_state& other) const = default;
    };

    struct cave_state_hash {
        template <size_t Depth>
        size_t operator()(const day23::cave_state<Depth> & x) const {
            size_t to_return = 0;
            for (size_t i = 0; i < x.hallway.size(); i++) {
                to_return += (i + 2) * (std::hash<char>()(x.hallway[i]) + i * 13);
            }
            for (size_t i = 0; i < x.caves.size(); i++) {
                for (size_t j = 0; j < Depth; j++) {
                    to_return += (i + 3) * (j + 5) * (std::hash<char>()(x.caves[i][j]) + i * 17 + j * 23);
                }
            }
            return to_return;
        }
    };

    int cost_multiplier(char c) {
        switch (c) {
            case 'A': return 1;
            case 'B': return 10;
            case 'C': return 100;
            case 'D': return 1000;
            default: return 0;
        }
    }

    template<int Depth>
    std::vector<std::pair<cave_state<Depth>, long>> get_neighbour_states(const cave_state<Depth>& c) {
        std::vector<std::pair<cave_state<Depth>, long>> to_return;

        constexpr std::array end_steps{0, 1, 3, 5, 7, 9, 10};
        auto not_empty_itr = stdv::filter([&c](auto i) { return c.hallway[*i] != '\0'; });
        auto not_empty = stdv::filter([&c](auto i) { return c.hallway[i] != '\0'; });

        auto add_change_hallway_to_state = [&](int old_position, int new_position) {
            auto cpy = c;
            std::swap(cpy.hallway[old_position], cpy.hallway[new_position]);
            to_return.push_back(
                   std::make_pair(
                          cpy, cost_multiplier(cpy.hallway[new_position]) * (std::abs(new_position - old_position))));
        };

        auto into_cave = [&](int index_from, char type) {
            auto cpy = c;
            auto& in_cave = cpy.caves[type - 'A'];
            auto non_correct = std::find_if(in_cave.rbegin(), in_cave.rend(), [type](char c) { return c != type; });
            bool correct = std::all_of(non_correct, in_cave.rend(), [](char c) { return c == '\0'; });
            if (correct) {
                std::swap(cpy.hallway[index_from], *non_correct);
                to_return.push_back(
                       std::make_pair(
                              cpy,
                              cost_multiplier(type) * (
                                     std::abs(index_from - 2 * (type - 'A' + 1)) + (non_correct.base() - in_cave.begin()))
                              ));
            }
        };

        auto from_cave = [&](int index_to, int cave_id, int cave_index) {
            if (c.hallway[index_to])
                return;
            auto cpy = c;
            std::swap(cpy.hallway[index_to], cpy.caves[cave_id][cave_index]);
            to_return.push_back(
                   std::make_pair(
                          cpy,
                          cost_multiplier(cpy.hallway[index_to]) * (cave_index + 2)));
        };

        // Move from hallway into cave
        for (auto hallway_position : end_steps | not_empty) {
            char crab = c.hallway[hallway_position];
            int target = (crab - 'A' + 1) * 2;
            if (target > hallway_position) {
                if (stdr::all_of(stdv::iota(hallway_position + 1, target + 1), [&](int i) { return !c.hallway[i]; } )) {
                    into_cave(hallway_position, crab);
                }
            } else {
                if (stdr::all_of(stdv::iota(target, hallway_position), [&](int i) { return !c.hallway[i]; } )) {
                    into_cave(hallway_position, crab);
                }
            }
        }

        if (to_return.empty()) {
            // Move from hallway
            for (auto hallway_position : end_steps | ox::ranges::views::iterators | not_empty_itr) {
                if (hallway_position != std::prev(end_steps.end()) && c.hallway[hallway_position[1]] == '\0') {
                    add_change_hallway_to_state(*hallway_position, hallway_position[1]);
                }
                if (hallway_position != end_steps.begin() && c.hallway[hallway_position[-1]] == '\0') {
                    add_change_hallway_to_state(*hallway_position, hallway_position[-1]);
                }
            }

            // Move from hallway
            for (auto& cave : c.caves | ox::ranges::views::iterators) {
                auto top = std::find_if(cave->begin(), cave->end(), std::identity());
                if (std::all_of(top, cave->end(), [&](char crab) { return crab == 'A' + (cave - c.caves.begin()); } ))
                    continue;
                if (top != cave->end()) {
                    int index = cave - c.caves.begin();
                    int hallway_index = 2 * (index + 1);
                    from_cave(hallway_index + 1, index, top - cave->begin());
                    from_cave(hallway_index - 1, index, top - cave->begin());
                }
            }
        }

        return to_return;
    }

    template <int Depth>
    long heuristic_distance_to_end(const cave_state<Depth>& state, const cave_state<Depth>&) {
        constexpr std::array end_steps{0, 1, 3, 5, 7, 9, 10};
        long cost = 0;

        for (int i = 0; i < state.caves.size(); i++) {
            char type = 'A' + i;
            int cave_x_position = (i + 1) * 2;

            auto& cave = state.caves[i];
            auto non_correct = std::find_if(cave.rbegin(), cave.rend(), [type](char c) { return c != type; });

            for (auto x : stdr::subrange(non_correct, cave.rend()) | ox::ranges::views::iterators) {
                int position = x.base() - cave.begin();
                cost += cost_multiplier(*x) * position;
                cost += cost_multiplier(type) * position;
                cost += cost_multiplier(*x) * std::abs(cave_x_position - (*x - 'A' + 1) * 2);
            }
        }

        for (auto x_pos : end_steps) {
            if (char type = state.hallway[x_pos]; type) {
                cost += cost_multiplier(type) * std::abs(x_pos - (type - 'A' + 1) * 2);
            }

        }

        return cost;
    }

    template<int Depth>
    void print_state(const cave_state<Depth>& cave) {
        for (char c : cave.hallway) {
            printf("%c", c ?: '.');
        }
        printf("\n");
#define GET_CHR_(i, j) cave.caves[i][j] ?: '.'
        for (int i : stdv::iota(0, Depth)) {
            printf("  %c %c %c %c\n", GET_CHR_(0, i), GET_CHR_(1, i), GET_CHR_(2, i), GET_CHR_(3, i));
        }
#undef GET_CHR_
    }

    void puzzle1() {
        cave_state<2> part1({'C', 'B'}, {'A', 'A'}, {'B', 'D'}, {'D', 'C'});

        auto [path, cost] = ox::dikstra(
               part1, cave_state<2>({'A', 'A'}, {'B', 'B'}, {'C', 'C'}, {'D', 'D'}),
               get_neighbour_states<2>,
               [](...) { return 0; }/*heuristic_distance_to_end<2>*/,
               cave_state_hash(),
               [](auto& c) {
                   printf("\033[2J");
                   printf("\033[1;1H");
                   print_state<2>(c);
               });

        for (auto& [state, cost] : path) {
            print_state<2>(state);
            printf("Cost is %ld\n\n", cost);
        }
        printf("Cost is %ld\n", cost);
    }

    void puzzle2() {
        cave_state<4> part1(
               {'C', 'D', 'D', 'B'},
               {'A', 'C', 'B', 'A'},
               {'B', 'B', 'A', 'D'},
               {'D', 'A', 'C', 'C'});

        auto [path, cost] = ox::dikstra(
               part1, cave_state<4>(
                             {'A', 'A', 'A', 'A'},
                             {'B', 'B', 'B', 'B'},
                             {'C', 'C', 'C', 'C'},
                             {'D', 'D', 'D', 'D'}),
               get_neighbour_states<4>,
               [](...) { return 0; },
               cave_state_hash());

        for (auto& [state, cost] : path) {
            print_state<4>(state);
            printf("Cost is %ld\n\n", cost);
        }
        printf("Cost is %ld\n", cost);
    }
} // namespace day23