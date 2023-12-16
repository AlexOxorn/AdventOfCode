#include "../../../common.h"
#include <ox/grid.h>
#include <numeric>

namespace aoc2023::day14 {
    struct rocks : public ox::grid<char> {
        using ox::grid<char>::grid;

        void print() const {
            leveled_foreach([](char c) { myprintf("%c", c); }, []() { myprintf("\n"); });
            printf("\n");
        }

        void tilt_north() {
            auto [w, h] = dimensions;

            for (long row = 1; row < h; ++row) {
                for (long col = 0; col < w; ++col) {
                    if ((*this)[col, row] != 'O')
                        continue;
                    for (long row2 = row - 1; row2 >= 0 && (*this)[col, row2] == '.'; --row2) {
                        std::swap((*this)[col, row2], (*this)[col, row2 + 1]);
                    }
                }
            }
        }

        void tilt_south() {
            auto [w, h] = dimensions;

            for (long row = h - 2; row >= 0; --row) {
                for (long col = 0; col < w; ++col) {
                    if ((*this)[col, row] != 'O')
                        continue;

                    for (long row2 = row + 1; row2 < h && (*this)[col, row2] == '.'; ++row2) {
                        std::swap((*this)[col, row2], (*this)[col, row2 - 1]);
                    }
                }
            }
        }
        void tilt_west() {
            auto [w, h] = dimensions;

            for (long row = 0; row < h; ++row) {
                for (long col = 1; col < w; ++col) {
                    if ((*this)[col, row] != 'O')
                        continue;
                    for (long col2 = col - 1; col2 >= 0 && (*this)[col2, row] == '.'; --col2) {
                        std::swap((*this)[col2, row], (*this)[col2 + 1, row]);
                    }
                }
            }
        }
        void tilt_east() {
            auto [w, h] = dimensions;

            for (long row = 0; row < h; ++row) {
                for (long col = w - 2; col >= 0; --col) {
                    if ((*this)[col, row] != 'O')
                        continue;

                    for (long col2 = col + 1; col2 < w && (*this)[col2, row] == '.'; ++col2) {
                        std::swap((*this)[col2, row], (*this)[col2 - 1, row]);
                    }
                }
            }
        }

        void cycle() {
            tilt_north();
            tilt_west();
            tilt_south();
            tilt_east();
        }
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto rr = get_stream(filename);
        rocks rrr(rr);
        rrr.tilt_north();
#ifdef __cpp_lib_ranges_chunk
        auto counts = stdv::enumerate(rrr | stdv::reverse) | stdv::transform([](const auto& row_index) {
            const auto& [index, row] = row_index;
            return (index + 1) * stdr::count(row, 'O');
        });
        long res = std::accumulate(counts.begin(), counts.end(), 0l);
#else
        long res = 0;
        int rank = int(rrr.get_height());
        for (const auto& row : rrr) {
            res += rank-- * stdr::count(row, 'O');
        }
#endif

        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto rr = get_stream(filename);
        rocks rrr(rr);
        std::vector<rocks> prev{rrr};

        long loop_start = 0;
        long loop_length = 0;

        for (long i = 0; i < 1'000'000'000; ++i) {
            rrr.cycle();
            if (auto index = stdr::find(prev, rrr); index != prev.end()) {
                loop_start = index - prev.begin();
                loop_length = prev.end() - index;
                break;
            }
            prev.push_back(rrr);
        }

        long total_loops = 1'000'000'000l;
        long loop_offset = (total_loops - loop_start) % loop_length;

        rocks& final = prev[loop_start + loop_offset];

#ifdef __cpp_lib_ranges_chunk
        auto counts = stdv::enumerate(final | stdv::reverse) | stdv::transform([](const auto& row_index) {
                          const auto& [index, row] = row_index;
                          return (index + 1) * stdr::count(row, 'O');
                      });
        long res = std::accumulate(counts.begin(), counts.end(), 0l);
#else
        long res = 0;
        int rank = int(final.get_height());
        for (const auto& row : final) {
            res += rank-- * stdr::count(row, 'O');
        }
#endif

        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day14