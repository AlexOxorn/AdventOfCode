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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
  ((byte) &0x40 ? '#' : '.'), ((byte) &0x20 ? '#' : '.'), ((byte) &0x10 ? '#' : '.'), ((byte) &0x08 ? '#' : '.'), \
          ((byte) &0x04 ? '#' : '.'), ((byte) &0x02 ? '#' : '.'), ((byte) &0x01 ? '#' : '.')

namespace aoc2022::day17 {
    using namespace ox::int_alias;
    using piece = std::array<u8, 4>;

    u32 from_bytes(u8* data) {
        return (data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24));
    }

    u32 from_bytes(piece data, long lshift) {
        u32 sum = 0;
        for (int i = 0; i < 4; ++i) {
            sum += (data[i] >> lshift) << (i * 8);
        }
        return sum;
    }

    constexpr piece _piece{0b111'1000};
    constexpr piece T_piece{0b010'0000, 0b111'0000, 0b010'0000};
    constexpr piece J_piece{0b111'0000, 0b001'0000, 0b001'0000};
    constexpr piece I_piece{0b100'0000, 0b100'0000, 0b100'0000, 0b100'0000};
    constexpr piece O_piece{0b110'0000, 0b110'0000};

    constexpr std::array<const piece, 5> pieces{_piece, T_piece, J_piece, I_piece, O_piece};

    struct board {
    private:
    public:
        std::vector<u8> spots;
    private:
        std::optional<piece> active_piece;
        long current_x = -1;
        long current_y = -1;
        long floor_y = 0;
        long max_y = 0;

        bool test_location() {
            if (current_x < 0)
                return false;
            if (current_y < floor_y)
                return false;
            if (stdr::any_of(*active_piece,
                             [this](u8 a) { return __builtin_popcount(a) != __builtin_popcount(a >> current_x); }))
                return false;
            return !(from_bytes(&spots[current_y - floor_y]) & from_bytes(*active_piece, current_x));
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
            for (int i = 0; i < 4; i++) {
                spots[current_y + i - floor_y] |= (*active_piece)[i] >> current_x;
            }
            auto max_new_piece = stdr::find_if(spots | stdv::reverse, std::identity());
            max_y = max_new_piece.base() - spots.begin() + floor_y;
            current_y = -1;
            current_x = -1;
            active_piece.reset();

            auto x = spots | stdv::reverse | stdv::take(10);
            if (auto res = stdr::find(x, 0b111'1111); res != x.end()) {
                //                print();
                floor_y += res.base() - spots.begin();
                std::vector new_spots(res.base(), spots.end());
                spots = std::move(new_spots);
            }
        }
    public:
        void print(int take_amount = 15) {
            for (unsigned char c : spots | stdv::reverse | stdv::take(15)) {
                printf("|" BYTE_TO_BINARY_PATTERN "|\n", BYTE_TO_BINARY(c));
            }
            printf("+-------+\n");
        }

        void spawn_piece(piece p) {
            long remaining = max_y + 3 + 5 - spots.size();
            if (remaining > 0) {
                spots.resize(spots.size() + remaining - floor_y);
            }
            active_piece = p;
            current_x = 2;
            current_y = max_y + 3;
        }

        void try_move_left() { try_move(&board::current_x, -1); }
        void try_move_right() { try_move(&board::current_x, 1); }
        void try_move_down() { try_move(&board::current_y, -1, true); }

        size_t get_max_height() { return max_y; }

        explicit operator bool() { return active_piece.has_value(); }
    };

    void solve(const char* filename, long num_of_block, bool type) {
        static std::vector<char> motions{get_from_input<char>(filename)};
        board tetris;
        auto motion_iter = motions.begin();
        auto piece_iter = pieces.begin();

        std::vector loop{100, 911, 479, 76, 149};

        long loop_size = std::accumulate(loop.begin(), loop.end(), 0l);
        long loop_start = 0;

        auto drop_block = [&, size = 0lu, loop_count = 0lu](...) mutable {
            ++loop_count;
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

            if (size > tetris.spots.size()) {
                printf("at block %zu > %zu: %zu | %zu\n", size, tetris.spots.size(), loop_start, loop_count);
                size = tetris.spots.size();
                loop_count = 0;
                return false;
            }

            size = tetris.spots.size();
            return true;
        };
        for (loop_start = 0; loop_start < num_of_block; ++loop_start) {
            if (!drop_block() && type)
                break;
        }

        printf("loop start : %zu\n", loop_start);

        long number_of_loops = (num_of_block - loop_start) / loop_size;
        long loop_remainder = (num_of_block - loop_start) % loop_size;

        printf("number_of_loops : %zu\n", number_of_loops);
        printf("loop_remainder : %zu\n", loop_remainder);

        size_t pre_loop_height = tetris.get_max_height();

        if (type) {
            stdr::for_each(stdv::iota(0l, loop_size), drop_block);
            size_t loop_height = tetris.get_max_height() - pre_loop_height;

            stdr::for_each(stdv::iota(0l, loop_remainder), drop_block);
            size_t remaining_height = tetris.get_max_height() - pre_loop_height - loop_height;

            printf("loop height : %zu\n", loop_height);
            printf("remaining height : %zu\n", remaining_height);

            printf("The height after %ld blocks is %ld\n==========================\n",
                   num_of_block,
                   pre_loop_height + (loop_height * number_of_loops) + remaining_height);
        } else {
            printf("The height after %ld blocks is %zu\n==========================\n",
                   num_of_block,
                   tetris.get_max_height());
        }
    }

    void puzzle1(const char* filename) {
        solve(filename, 2022, true);
        solve(filename, 2022, false);
    }

    void puzzle2(const char* filename) {
//        solve(filename, 1'000'000'000'000);
    }
} // namespace aoc2022::day17