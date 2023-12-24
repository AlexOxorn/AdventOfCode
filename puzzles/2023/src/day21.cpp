#include "../../../common.h"
#include <ox/grid.h>
#include <ox/math.h>
#include <set>
#include <unordered_set>
#include <string_view>
#include <concepts>
#include <format>

namespace aoc2023::day21 {

    struct flower_map : ox::grid<char> {
        using ox::grid<char>::grid;
        using ox::grid<char>::const_raw_iterator;

        [[nodiscard]] const_raw_iterator find_start() const { return stdr::find(data, 'S'); }

        void print(const std::set<flower_map::const_raw_iterator>& highlight) const {
            leveled_iterators(
                    [&](auto c) {
                        if (highlight.contains(c)) {
                            printf("\033[41m ");
                        } else {
                            printf("\033[0m%c", *c);
                        }
                    },
                    []() { printf("\033[0m\n"); });
        }

        void print(const std::set<flower_map::const_raw_iterator>& highlight,
                   const std::set<flower_map::const_raw_iterator>& seen) const {
            leveled_iterators(
                    [&](auto c) {
                        if (highlight.contains(c)) {
                            printf("\033[41m ");
                        } else if (seen.contains(c)) {
                            printf("\033[42m ");
                        } else {
                            printf("\033[0m%c", *c);
                        }
                    },
                    []() { printf("\033[0m\n"); });
        }
    };

    template <typename T>
    concept trivially_copyable = std::is_trivially_copyable_v<T>;

    static auto trivial_hash = []<trivially_copyable T>(const T& data) {
        std::string_view ss((char*) &data, sizeof(data));
        return std::hash<std::string_view>()(ss);
    };

    static auto vector_hash = [](const auto& data) {
        std::string ss = std::format("{}", data);
        return std::hash<std::string>()(ss);
    };

    using positions = std::set<flower_map::const_raw_iterator>;
    using positions2 = std::unordered_set<std::array<long, 2>, decltype(trivial_hash)>;
    template <size_t N = 5>
    using start_cache = std::unordered_map<std::array<long, N>, long, decltype(trivial_hash)>;
    using start_cache2 = std::unordered_map<std::vector<long>, long, decltype(vector_hash)>;
    using positions3 = std::unordered_map<std::array<long, 2>, long, decltype(trivial_hash)>;

    long calculate_steps(flower_map& map, flower_map::const_raw_iterator start, long num_steps) {
        positions curr{start};

        for (long count = 0; count < num_steps; ++count) {
            positions next;
            for (auto pos : curr) {
                auto neighbours = map.cardinal_neighbour_range(pos);
                for (auto adj : neighbours | flower_map::const_valid_index()) {
                    if (*adj == '.' or *adj == 'S') {
                        next.insert(adj);
                    }
                }
            }
            std::swap(curr, next);
            next.clear();
        }

        return curr.size();
    }

    std::vector<positions> visual_steps(flower_map& map, flower_map::const_raw_iterator start, long max = 64,
                                        bool repeat = false) {
        positions curr{start};
        positions seen{start};
        std::vector<positions> past{curr};
        size_t previous_seen_count = 0;
        for (long count = 0; count < max and !curr.empty() and previous_seen_count != seen.size(); ++count) {
            previous_seen_count = seen.size();
            positions next;
            for (auto pos : curr) {
                auto neighbours = map.cardinal_neighbour_range(pos);
                for (auto adj : neighbours | flower_map::const_valid_index()) {
                    if ((*adj == '.' or *adj == 'S') and (repeat or !seen.contains(adj))) {
                        next.insert(adj);
                        seen.insert(adj);
                    }
                }
            }
            std::swap(curr, next);
            past.push_back(curr);
            map.print(curr, seen);
            next.clear();
            printf("\n");
        }

        return past;
    }

    size_t calculate_steps2(const flower_map& map, flower_map::const_raw_iterator start, long num_steps) {
        auto start_pos = map.coord_from_index(start);
        positions2 curr{start_pos};
        positions3 seen{{start_pos, map.get_size()}};
        positions2 next;

        bool even = (num_steps % 2 == 0) != ((start_pos[0] + start_pos[1]) % 2 == 0);
        long res = 0;

        for (long count = 0; count < num_steps; ++count) {
            for (auto pos : curr) {
                std::array<std::array<long, 2>, 4> neighbours{
                        std::array{pos[0] - 1, pos[1]    },
                        {pos[0] + 1, pos[1]    },
                        {pos[0],     pos[1] - 1},
                        {pos[0],     pos[1] + 1}
                };
                for (auto adj : neighbours) {
                    if (seen.contains(adj))
                        continue;
                    std::array<long, 2> bounded{ox::mod(adj[0], long(map.get_width())),
                                                ox::mod(adj[1], long(map.get_height()))};
                    if (auto val = map[bounded]; val == '.' || val == 'S') {
                        if ((adj[0] + adj[1]) % 2 == even) {
                            ++res;
                        }
                        next.insert(adj);
                        seen[adj] = map.get_size();
                    }
                }
            }

            for (auto& [x, v]: seen) {
                --v;
            }
            std::erase_if(seen, [](auto& x) {
               return x.second <= 0;
            });

//            printf("%8ld -> %zu\n", count, curr.size());
            std::swap(curr, next);
            next.clear();
            next.reserve(curr.size() * 2);
        }

        return res;
    }

    enum DIR : long { NONE, NORTH = 1 << 1, EAST = 1 << 2, WEST = 1 << 3, SOUTH = 1 << 4 };

    long update_forbidden(long init, DIR from) {
        long to_return = 0;
        switch (from) {
            case NONE: std::unreachable();
            case NORTH:

            case SOUTH:
            case WEST:
            case EAST:
                return 0;
        }
        return 0;
    }

/*    auto calculate_steps3(start_cache<4>& cache, const flower_map& map, std::array<long, 2> start, long forbid,
                          long num_steps) {
        std::array key{start[0], start[1], num_steps, forbid};

        if (cache.contains(key)) {
            //            printf("CACHE HIT\n");
            return cache.at(key);
        }
        //        printf("CACHE MISS\n");

        positions2 curr{start};
        positions2 next;

        long res = 0;
        for (long count = 0; count < num_steps; ++count) {
            for (auto pos : curr) {
                std::array<std::array<long, 2>, 4> neighbours{
                        std::array{pos[0] - 1, pos[1]    },
                        {pos[0] + 1, pos[1]    },
                        {pos[0],     pos[1] - 1},
                        {pos[0],     pos[1] + 1}
                };
                for (auto adj : neighbours) {
                    if (auto x = map.get(adj); x) {
                        if (x == '.' || x == 'S') {
                            next.insert(adj);
                        }
                    } else {
                        DIR to;
                        std::array<long, 2> new_world = from.back();
                        if (adj[0] >= long(map.get_width())) {
                            ++new_world[0];
                        }
                        if (adj[0] < 0l) {
                            --new_world[0];
                        }
                        if (adj[1] >= long(map.get_height())) {
                            ++new_world[1];
                        }
                        if (adj[1] < 0l) {
                            --new_world[1];
                        }
                        if (stdr::find(from, new_world) != from.end())
                            continue;

                        from.push_back(new_world);
                        std::array<long, 2> bounded{ox::mod(adj[0], long(map.get_width())),
                                                    ox::mod(adj[1], long(map.get_height()))};
                        if (auto val = map[bounded]; val == '.' || val == 'S') {
                            res += calculate_steps3(cache, map, bounded, from, num_steps - (count + 1));
                        }
                        from.pop_back();
                    }
                }
            }

            std::swap(curr, next);
            next.clear();
            next.reserve(curr.size() * 2);
        }

        long ans = res + long(curr.size());
        //        printf("CACHED\n");
        cache[{start[0], start[1], num_steps}] = ans;

        return ans;
    }

    long calculate_steps4(start_cache& cache, const flower_map& map, std::array<long, 2> pos, std::array<long, 2> world,
                          long num_steps) {
        std::array key{pos[0], pos[1], world[0], world[1], num_steps};

        if (num_steps == 0) {
            cache[key] = 1;
            return 1;
        }
        if (cache.contains(key)) {
            return 0;
            return cache.at(key);
        }
        long res = 0;
        std::array<std::array<long, 2>, 4> neighbours{
                std::array{pos[0] - 1, pos[1]    },
                {pos[0] + 1, pos[1]    },
                {pos[0],     pos[1] - 1},
                {pos[0],     pos[1] + 1}
        };
        for (auto adj : neighbours) {
            if (auto x = map.get(adj); x) {
                if (x == '.' || x == 'S') {
                    res += calculate_steps4(cache, map, adj, world, num_steps - 1);
                }
            } else {
                std::array<long, 2> bounded{ox::mod(adj[0], long(map.get_width())),
                                            ox::mod(adj[1], long(map.get_height()))};
                if (auto val = map[bounded]; val == '#') {
                    continue;
                }
                std::array<long, 2> new_world = world;
                if (adj[0] >= long(map.get_width())) {
                    ++new_world[0];
                }
                if (adj[0] < 0l) {
                    --new_world[0];
                }
                if (adj[1] >= long(map.get_height())) {
                    ++new_world[1];
                }
                if (adj[1] < 0l) {
                    --new_world[1];
                }

                res += calculate_steps4(cache, map, bounded, new_world, num_steps - 1);
            }
        }

        cache[key] = res;
        return res;
    }*/

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        flower_map map(input);
        long res = calculate_steps(map, map.find_start(), 64);
        printf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        flower_map map(input);

        /*
         *
In exactly 6 steps, he can still reach 16 garden plots.
In exactly 10 steps, he can reach any of 50 garden plots.
In exactly 50 steps, he can reach 1594 garden plots.
In exactly 100 steps, he can reach 6536 garden plots.
In exactly 500 steps, he can reach 167004 garden plots.
In exactly 1000 steps, he can reach 668697 garden plots.
In exactly 5000 steps, he can reach 16733044 garden plots.

         */
        std::array steps{6, 10, 50, 100, 500, 1000, 5000};
        auto start = map.coord_from_index(map.find_start());
        for (long step : steps) {
            start_cache<> cache;
            std::array start_world{0l, 0l};
            std::vector from{start_world};
            size_t res = calculate_steps2(map, map.find_start(), step);
            printf("%ld -> %zu\n", step, res+1);
        }
        size_t res = calculate_steps2(map, map.find_start(), 26501365);
        printf("%ld -> %zu\n", 26501365l, res + 1);
        return res+1;
    }
} // namespace aoc2023::day21