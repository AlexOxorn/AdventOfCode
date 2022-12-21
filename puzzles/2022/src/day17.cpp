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
#include <ox/grid.h>
#include <ox/types.h>
#include <ox/hash.h>

namespace aoc2022::day17 {
    using namespace ox::int_alias;

    using coord = std::pair<long, long>;
    using piece = std::array<coord, 5>;
    coord operator+(const coord& x, const coord& y) {
        return std::make_pair(x.first + y.first, x.second + y.second);
    }

    constexpr piece _piece{
            {{0, 0}, {1, 0}, {2, 0}, {3, 0}}
    };
    constexpr piece T_piece{
            {{1, 2}, {0, 1}, {1, 1}, {2, 1}, {1, 0}}
    };
    constexpr piece J_piece{
            {{2, 2}, {2, 1}, {0, 0}, {1, 0}, {2, 0}}
    };
    constexpr piece I_piece{
            {{0, 3}, {0, 2}, {0, 1}, {0, 0}}
    };
    constexpr piece O_piece{
            {{0, 1}, {1, 1}, {0, 0}, {1, 0}}
    };

    constexpr std::array<const piece, 5> pieces{_piece, T_piece, J_piece, I_piece, O_piece};

    struct board {
    private:
        std::unordered_set<std::pair<long, long>, ox::pair_hash<long, long>> spots;
        std::optional<piece> active_piece;
        long current_x = -1;
        long current_y = -1;
        long max_y = 0;

        bool test_location() {
            return stdr::none_of(*active_piece, [this](const auto& pair) -> bool {
                auto x = std::make_pair(current_x + pair.first, current_y + pair.second);
                if (x.first < 0) return true;
                if (x.first >= 7) return true;
                if (x.second < 0) return true;
                return spots.contains(std::make_pair(current_x + pair.first, current_y + pair.second));
            });
        }

        void try_move(long board::*pos, long dir, bool set = false) {
            this->*pos += dir;
            if (!test_location()) {
                this->*pos -= dir;
                if (set) {
                    return set_piece();
                }
            }
        }

        void set_piece() {
            for (const auto& [x, y] : *active_piece) {
                spots.emplace(std::make_pair(current_x + x, current_y + y));
            }
            auto max_new_piece = stdr::max(*active_piece | stdv::values
                                               | stdv::transform(std::bind_front(std::plus<>(), current_y)));
            max_y = std::max(max_y, max_new_piece + 1);
            current_y = -1;
            current_x = -1;
            active_piece.reset();
        }
    public:
        /*void print() {
            printf("|");
            for (long j = static_cast<long>(get_height()) - 1; j >= 0; j--) {
                for (long i = 0; i < width; i++) {
                    bool found = false;
                    if (active_piece)
                        for (auto& [x, y] : *active_piece) {
                            if (x + current_x == i && y + current_y == j) {
                                printf("\033[%dm@\033[0m", 34);
                                found = true;
                                break;
                            }
                        }
                    if (!found)
                        printf("\033[%dm%c\033[0m", at(i, j) ? 31 : 0, at(i, j) ? '#' : '.');
                }
                printf("|\n|");
            }
            printf("\b+-------+\n");
        }*/

        void spawn_piece(piece p) {
            active_piece = std::move(p);
            current_x = 2;
            current_y = max_y + 3;
        }

        void try_move_left() { try_move(&board::current_x, -1); }
        void try_move_right() { try_move(&board::current_x, 1); }
        void try_move_down() { try_move(&board::current_y, -1, true); }

        long get_max_height() {
            return max_y;
        }

        explicit operator bool() { return active_piece.has_value(); }
    };

    void solve(const char* filename, long num_of_block) {
        static std::vector<char> motions{get_from_input<char>(filename)};
        board tetris;
        auto motion_iter = motions.begin();
        auto piece_iter = pieces.begin();
        for (long i = 0; i < num_of_block; i++) {
            if (i % 1'000'000 == 0) {
                printf("at %ld\n", i);
            }
            tetris.spawn_piece(*piece_iter);
            while (tetris) {
                switch (*motion_iter) {
                    case '<': tetris.try_move_left(); break;
                    case '>': tetris.try_move_right(); break;
                }
                tetris.try_move_down();
                if (++motion_iter == motions.end())
                    motion_iter = motions.begin();
            }

            if (++piece_iter == pieces.end())
                piece_iter = pieces.begin();
        }
        printf("The height after %ld blocks is %ld\n", num_of_block, tetris.get_max_height());
    }

    void puzzle1(const char* filename) {
        solve(filename, 2022);
    }

    void puzzle2(const char* filename) {
        solve(filename, 1'000'000'000'000);
    }
} // namespace aoc2022::day17