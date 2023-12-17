#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <functional>

namespace aoc2023::day17 {
    enum DIR : int32_t { START, LEFT, RIGHT, UP, DOWN };

    template <bool part2>
    struct state;

    struct block_map : ox::grid<int> {
        using move_sig =
                std::optional<block_map::const_raw_iterator> (block_map::*)(block_map::const_raw_iterator) const;

        constexpr static auto up_method = static_cast<move_sig>(&block_map::up);
        constexpr static auto down_method = static_cast<move_sig>(&block_map::down);
        constexpr static auto left_method = static_cast<move_sig>(&block_map::left);
        constexpr static auto right_method = static_cast<move_sig>(&block_map::right);

        constexpr static std::array forward_array{up_method, left_method, right_method, up_method, down_method};
        constexpr static std::array right_array{START, UP, DOWN, RIGHT, LEFT};
        constexpr static std::array left_array{START, DOWN, UP, LEFT, RIGHT};

        using ox::grid<int>::grid;

        void print() const {
            leveled_foreach([](int t) { myprintf("%d", t); }, []() { myprintf("\n"); });
            myprintf("\n");
        }

        template <bool part2>
        void print(const std::vector<std::pair<state<part2>, long>>& path) const {
            auto states_stream = path | stdv::keys;
            std::vector<state<part2>> states(states_stream.begin(), states_stream.end());

            auto head = data.begin();
            auto end = data.end();
            auto width = dimensions[0];

            for (; head != end; ++head) {
                if (head != data.begin() && (head - data.begin()) % width == 0) {
                    myprintf("\n");
                }
                if (auto x = stdr::find(states, head, [](const auto& s) { return s.pos; }); x != states.end()) {
                    myprintf("\033[31m");
                    switch (x->dir) {
                        case LEFT: myprintf("%c", '<'); break;
                        case RIGHT: myprintf("%c", '>'); break;
                        case UP: myprintf("%c", '^'); break;
                        case DOWN: myprintf("%c", 'v'); break;
                        case START: myprintf("%c", 'S'); break;
                        default: std::unreachable();
                    }
                } else {
                    myprintf("\033[0m%d", *head);
                }
            }

            myprintf("\n");
        }

        [[nodiscard]] auto move(block_map::const_raw_iterator start, DIR dir) const {
            return std::invoke(forward_array[dir], *this, start);
        }

        [[nodiscard]] auto move_left(block_map::const_raw_iterator start, DIR dir) const {
            DIR new_dir = left_array[dir];
            return std::pair(std::invoke(forward_array[new_dir], *this, start), new_dir);
        }

        [[nodiscard]] auto move_right(block_map::const_raw_iterator start, DIR dir) const {
            DIR new_dir = right_array[dir];
            return std::pair(std::invoke(forward_array[new_dir], *this, start), new_dir);
        }
    };

    template <bool part2>
    struct state {
        DIR dir{};
        int32_t consecutive_straight{};
        block_map::const_raw_iterator pos;

        bool operator==(const state&) const = default;
    };

    auto state_hash = [] (const auto& s) {
        std::string_view data((char*) &s, sizeof(s));
        return std::hash<std::string_view>()(data);
    };

    template <bool part2>
    std::vector<std::pair<state<part2>, long>> get_neighbours(const block_map* map, const state<part2>& s) {
        std::vector<std::pair<state<part2>, long>> to_return;
        if (s.dir == START) {
            auto down = map->move(s.pos, DOWN);
            auto right = map->move(s.pos, RIGHT);
            if (down) {
                to_return.emplace_back(state<part2>{DOWN, s.consecutive_straight + 1, *down}, **down);
            }
            if (right) {
                to_return.emplace_back(state<part2>{RIGHT, s.consecutive_straight + 1, *right}, **right);
            }
            return to_return;
        }

        if (s.consecutive_straight < (part2 ? 10 : 3)) {
            auto next = map->move(s.pos, s.dir);
            if (next)
                to_return.emplace_back(state<part2>{s.dir, s.consecutive_straight + 1, *next}, **next);
        }
        if (s.consecutive_straight >= (part2 ? 4 : 0)) {
            auto [next, new_dir] = map->move_left(s.pos, s.dir);
            if (next)
                to_return.emplace_back(state<part2>{new_dir, 1, *next}, **next);
        }
        if (s.consecutive_straight >= (part2 ? 4 : 0)) {
            auto [next, new_dir] = map->move_right(s.pos, s.dir);
            if (next)
                to_return.emplace_back(state<part2>{new_dir, 1, *next}, **next);
        }
        return to_return;
    }

    bool operator==(const state<false>& s, const block_map::const_raw_iterator& e) {
        return s.pos == e;
    }

    bool operator==(const state<true>& s, const block_map::const_raw_iterator& e) {
        return s.pos == e && s.consecutive_straight >= 4;
    }

    template <bool part2>
    answertype solve (puzzle_options filename) {
        auto stream = get_stream(filename);
        block_map bb(stream, [](char c) { return c - '0'; });
        ox::dikstra_solver solver(state<part2>{START, 0, bb.data.begin()},
                                  bb.data.end() - 1,
                                  std::bind_front(get_neighbours<part2>, &bb),
                                  state_hash);
        auto [path, cost] = solver();
        myprintf("%ld\n", cost);
        return cost;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        return solve<false>(filename);
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        return solve<true>(filename);
    }
} // namespace aoc2023::day17