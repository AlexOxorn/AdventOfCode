//
// Created by alexoxorn on 2022-01-03.
//

#include "day25.h"
#include <ox/grid.h>
#include <chrono>
#include <thread>

#define YEAR 2021
#define DAY  25

namespace day25 {
    enum class space : char { EMPTY, SOUTH_FACING, RIGHT_FACING, ERROR };

    class sea_floor : public ox::grid<space> {
        using ox::grid<space>::grid;

        void move_right_row(const std::pair<raw_iterator, raw_iterator>& row) {
            const auto& [start, end] = row;
            bool start_empty = *start == space::EMPTY;

            // Determine which cucumbers can move
            for (auto temp_start = start; temp_start != end; ++temp_start) {
                if (*temp_start != space::RIGHT_FACING)
                    continue;
                if (auto move_location = right(std::optional(temp_start)); move_location) {
                    if (**move_location == space::EMPTY) {
                        std::swap(*temp_start, **move_location);
                        ++temp_start;
                    }
                } else {
                    if (start_empty)
                        std::swap(*temp_start, *start);
                }
            }
        }

        void move_south_column(int column_id) {
            const auto start = data.begin() + column_id;
            bool start_empty = *start == space::EMPTY;

            // Determine which cucumbers can move
            for (auto temp_start = start; temp_start < data.end(); temp_start += width) {
                if (*temp_start != space::SOUTH_FACING)
                    continue;
                if (auto move_location = down(std::optional(temp_start)); move_location) {
                    if (**move_location == space::EMPTY) {
                        std::swap(*temp_start, **move_location);
                        temp_start += width;
                    }
                } else {
                    if (start_empty)
                        std::swap(*temp_start, *start);
                }
            }
        }

        void move_right() {
            for (std::pair<raw_iterator, raw_iterator> row : *this) {
                move_right_row(row);
            }
        }

        void move_south() {
            for (int i : stdv::iota(0, width)) {
                move_south_column(i);
            }
        }
    public:
        void move() {
            move_right();
            move_south();
        }

        void print_array() {
            leveled_foreach(
                    [](auto& elem) {
                        printf("%c",
                               elem == space::EMPTY          ? '.'
                               : elem == space::RIGHT_FACING ? '>'
                               : elem == space::SOUTH_FACING ? 'v'
                                                             : 'X');
                    },
                    []() { printf("\n"); });
            printf("\n");
        }
    };

    void puzzle1() {
        using namespace std::chrono_literals;
        sea_floor cucumbers(GET_STREAM(input, ox::line), [](char a) {
            switch (a) {
                case '.': return space::EMPTY;
                case '>': return space::RIGHT_FACING;
                case 'v': return space::SOUTH_FACING;
                default: return space::ERROR;
            }
        });

        auto prev = cucumbers;
        int i = 1;
        for (;; ++i) {
            printf("\033[2J\033[1;1H");
            cucumbers.print_array();
            cucumbers.move();
            if (prev == cucumbers)
                break;
            prev = cucumbers;
            std::this_thread::sleep_for(60ms);
        }

        printf("Stops moving after %d step\n", i);
    }

    void puzzle2() {}
} // namespace day25