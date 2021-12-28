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
    constexpr std::array<int, 7> valid_positions{0, 1, 3, 5, 7, 9, 10};
    constexpr std::array<int, 4> pit_positions{2, 4, 6, 8};
    using cave = ox::grid<char>;

    struct position {
        int x;
        int y;

        auto operator<=>(const position& other) const = default;
    };

    using crab = std::pair<position, char>;
    using state = std::array<crab, 8>;

    struct crab_hash {
        size_t operator()(const crab& s) const {
            return std::hash<int>()(s.first.x) + std::hash<int>()(s.first.y) + std::hash<char>()(s.second);
        }
        size_t operator()(const state& s) const {
            return std::accumulate(s.begin(), s.end(), std::size_t(), [this](size_t partial_sum, const crab& c) { return partial_sum + (*this)(c); });
        }
    };

    int type_multiplier(char type) {
        switch (type) {
            case 'A': return 1;
            case 'B': return 10;
            case 'C': return 100;
            case 'D': return 1000;
            default: return 0;
        }
    }

    int distance(int x1, int y1, int x2, int y2, char type) {
        return type_multiplier(type) * (std::abs(y2 + y1) + std::abs(x2 - x1));
    }

    void try_position(int x, const crab& current, std::vector<std::pair<state, int>>& to_return, const state& s, int def = 2) {
        switch (x) {
            case 2:
            case 4:
            case 6:
            case 8:
                if (2 * (current.second - 'A' + 1) == x) {
                    auto crabs_in_bucket = s | stdv::filter([x](const crab& c) { return c.first.y != 0 && c.first.x == x; });
                    if (stdr::all_of(crabs_in_bucket, [&current](const crab& c) { return c.second == current.second; })) {
                        auto min_element = std::max_element(crabs_in_bucket.begin(), crabs_in_bucket.end(), [](const crab& l, const crab& r) { return l.first.y < r.first.y; });
                        auto new_y = min_element == crabs_in_bucket.end() ? def : min_element->first.y - 1;
                        auto cpy = s;
                        auto iter_updated = std::find(cpy.begin(), cpy.end(), current);
                        iter_updated->first = {x, new_y};
                        to_return.emplace_back(std::move(cpy), distance(current.first.x, current.first.y, x, new_y, current.second));
                    }
                }
                break;
            default:
                printf("This should not appear\n");
                break;
            case 0:
            case 1:
            case 3:
            case 5:
            case 7:
            case 9:
            case 10:
                auto cpy = s;
                auto iter_updated = std::find(cpy.begin(), cpy.end(), current);
                iter_updated->first = {x, 0};
                to_return.emplace_back(std::move(cpy), distance(current.first.x, current.first.y, x, 0, current.second));
                break;
        }
    }

    template <int depth = 2>
    std::vector<std::pair<state, int>> get_state_neighbours(const state& s) {
        std::vector<std::pair<state, int>> to_return;
        for (const crab& current_crab : s) {
            bool final_spot = (2 * (current_crab.second - 'A' + 1) == current_crab.first.x);
            final_spot = final_spot && stdr::all_of(s, [&current_crab](const crab& c) {
                if (c == current_crab)
                    return true;
                if (c.first.x == current_crab.first.x)
                    return current_crab.first.y > c.first.y || c.second == current_crab.second;
                return true;
            });
            if (final_spot)
                continue;

            bool can_access_top = current_crab.first.y == 0;
            can_access_top = can_access_top || stdr::all_of(s, [&current_crab](const crab& c) {
                if (c == current_crab)
                    return true;
                if (c.first.x == current_crab.first.x)
                    return current_crab.first.y < c.first.y;
                return true;
            });
            if (can_access_top) {
                auto no_interference = [&s](int x) {
                    return std::find_if(s.begin(), s.end(), [x](auto& c) { return c.first.x == x && c.first.y == 0; }) == s.end();
                };
                auto possible_to_right = stdv::iota(current_crab.first.x + 1, 11)
                       | stdv::take_while(no_interference);
                auto possible_to_left = stdv::iota(0, current_crab.first.x)
                       | stdv::reverse
                       | stdv::take_while(no_interference);
                for (int x_positions : possible_to_right) {
                    try_position(x_positions, current_crab, to_return, s, depth);
                }
                for (int x_positions : possible_to_left) {
                    try_position(x_positions, current_crab, to_return, s, depth);
                }
            }
        }
        return to_return;
    }

    void puzzle1() {
        constexpr int depth = 2;
//        state start{{
//               {{2, 1}, 'C'},
//               {{2, 2}, 'B'},
//               {{4, 1}, 'A'},
//               {{4, 2}, 'A'},
//               {{6, 1}, 'B'},
//               {{6, 2}, 'D'},
//               {{8, 1}, 'D'},
//               {{8, 2}, 'C'}}};
                state start{{
                       {{2, 1}, 'B'},
                       {{2, 2}, 'A'},
                       {{4, 1}, 'A'},
                       {{4, 2}, 'B'},
                       {{6, 1}, 'C'},
                       {{6, 2}, 'C'},
                       {{8, 1}, 'D'},
                       {{8, 2}, 'D'}}};

        state end{{
               {{2, 1}, 'A'},
               {{2, 2}, 'A'},
               {{4, 1}, 'B'},
               {{4, 2}, 'B'},
               {{6, 1}, 'C'},
               {{6, 2}, 'C'},
               {{8, 1}, 'D'},
               {{8, 2}, 'D'}}};

        auto [path, cost] = ox::dikstra(
               start, end,
               get_state_neighbours,
               [](const state& s, const state&) {
                   return 0l;
                   return std::accumulate(s.begin(), s.end(), 0l, [&s](long sum, const crab& c) {
                       int final = 2 * (c.second - 'A' + 1);
                       if (final != c.first.x) {
                           return sum + (c.first.y + 1 + std::abs(final - c.first.x)) * type_multiplier(c.second);
                       } else {
                           int under = stdr::count_if(s, [&c](const crab& other) {
                               return (other.first.x == c.first.x && other.first.y > c.first.y && other.second != c.second);
                           });
                           return under > 0 ? (2 * c.first.y + 2 + depth) * type_multiplier(c.second) : 0l;
                       }
                   });
               },
               crab_hash());

        printf("Final Cost %d\n", cost);
    }

    void puzzle2() {
        /*state start{
               {{2, 1}, 'C'},
               {{2, 2}, 'D'},
               {{2, 3}, 'D'},
               {{2, 4}, 'B'},
               {{4, 1}, 'A'},
               {{4, 2}, 'C'},
               {{4, 3}, 'B'},
               {{4, 4}, 'A'},
               {{6, 1}, 'B'},
               {{6, 2}, 'B'},
               {{6, 3}, 'A'},
               {{6, 4}, 'D'},
               {{8, 1}, 'D'},
               {{8, 2}, 'A'},
               {{8, 3}, 'C'},
               {{8, 4}, 'C'}};

        state end{
               {{2, 1}, 'A'},
               {{2, 2}, 'A'},
               {{2, 3}, 'A'},
               {{2, 4}, 'A'},
               {{4, 1}, 'B'},
               {{4, 2}, 'B'},
               {{4, 3}, 'B'},
               {{4, 4}, 'B'},
               {{6, 1}, 'C'},
               {{6, 2}, 'C'},
               {{6, 3}, 'C'},
               {{6, 4}, 'C'},
               {{8, 1}, 'D'},
               {{8, 2}, 'D'},
               {{8, 3}, 'D'},
               {{8, 4}, 'D'}};

        auto [path, cost] = ox::dikstra(
               start, end,
               get_state_neighbours,
               crab_hash());*/
    }
} // namespace day23