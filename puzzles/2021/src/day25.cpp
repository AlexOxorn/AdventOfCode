#include "../../../common.h"
#include "ox/grid.h"
#include <chrono>
#include <thread>

#define DAY  25

namespace aoc2021::day25 {
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
            for (auto temp_start = start; temp_start < data.end(); temp_start += get_width()) {
                if (*temp_start != space::SOUTH_FACING)
                    continue;
                if (auto move_location = down(std::optional(temp_start)); move_location) {
                    if (**move_location == space::EMPTY) {
                        std::swap(*temp_start, **move_location);
                        temp_start += get_width();
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
            for (int i : stdv::iota(0ul, get_width())) {
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
                        myprintf("%c",
                               elem == space::EMPTY          ? '.'
                               : elem == space::RIGHT_FACING ? '>'
                               : elem == space::SOUTH_FACING ? 'v'
                                                             : 'X');
                    },
                    []() { myprintf("\n"); });
            myprintf("\n");
        }
    };

    answertype puzzle1(puzzle_options filename) {
        using namespace std::chrono_literals;
        sea_floor cucumbers(get_stream<ox::line>(filename), [](char a) {
            switch (a) {
                case '.': return space::EMPTY;
                case '>': return space::RIGHT_FACING;
                case 'v': return space::SOUTH_FACING;
                default: return space::ERROR;
            }
        });

        cucumbers.print_array();

        auto prev = cucumbers;
        int i = 1;
        for (;; ++i) {
            myprintf("\033[2J\033[1;1H");
            cucumbers.print_array();
            cucumbers.move();
            if (prev == cucumbers)
                break;
            prev = cucumbers;
            //std::this_thread::sleep_for(60ms);
        }

        myprintf("Stops moving after %d step\n", i);
        return i;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) { return {}; }
} // namespace aoc2021::day25