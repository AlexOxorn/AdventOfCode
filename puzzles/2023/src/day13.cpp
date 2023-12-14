#include "../../../common.h"
#include <ox/grid.h>
#include <numeric>

namespace aoc2023::day13 {
    struct maze : public ox::grid<char> {
        using ox::grid<char>::grid;

        [[nodiscard]] bool is_mirrored_vertically(int col, int allowed_smudge = 0) const {
            // mirrored between col - 1 and col
            int smudge_count = 0;

            for (long col_index = 0;; ++col_index) {
                auto left = get(col - (col_index + 1), 0);
                auto right = get(col + col_index, 0);
                if (!left || !right)
                    break;
                for (long row = 0; row < dimensions[1]; ++row) {
                    auto left = (*this)[col - (col_index + 1), row];
                    auto right = (*this)[col + col_index, row];
                    if (left != right) {
                        ++smudge_count;
                    }
                    if (smudge_count > allowed_smudge) {
                        return false;
                    }
                }
            }

            return smudge_count == allowed_smudge;
        }

        [[nodiscard]] bool is_mirrored_horizontally(int row, int allowed_smudge = 0) const {
            // mirrored between row - 1 and row
            int smudge_count = 0;

            for (long row_index = 0;; ++row_index) {
                auto left = get(0l, row - (row_index + 1));
                auto right = get(0l, row + row_index);
                if (!left || !right)
                    break;
                for (long col = 0; col < dimensions[0]; ++col) {
                    auto left = (*this)[col, row - (row_index + 1)];
                    auto right = (*this)[col, row + row_index];
                    if (left != right) {
                        ++smudge_count;
                    }
                    if (smudge_count > allowed_smudge) {
                        return false;
                    }
                }
            }

            return smudge_count == allowed_smudge;
        }

        [[nodiscard]] long find_mirror(int smudge = 0) const {

            long sum = 0;
            for(int col = 1; col < dimensions[0]; ++col) {
                if (is_mirrored_vertically(col, smudge)) {
                    return col;
                }
            }
            for(int row = 1; row < dimensions[1]; ++row) {
                if (is_mirrored_horizontally(row, smudge)) {
                    return 100 * row;
                }
            }
            return sum;
        }
    };

    std::istream& operator>>(std::istream& in, maze& m) {
        if (in.eof()) {
            in.setstate(std::ios::failbit);
            return in;
        }
        auto rows = stdv::istream<ox::line>(in) | stdv::take_while(std::not_fn(&std::string::empty));
        m = maze(rows, std::identity());
        if (in.eof())
            in.clear(std::istream::eofbit);
        return in;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto data = get_stream<maze>(filename);
        auto mirror_notes = data | stdv::transform([](const maze& m) { return m.find_mirror(0); } );
        long res = std::accumulate(mirror_notes.begin(), mirror_notes.end(), 0l);
        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto data = get_stream<maze>(filename);
        auto mirror_notes = data | stdv::transform([](const maze& m) { return m.find_mirror(1); } );
        long res = std::accumulate(mirror_notes.begin(), mirror_notes.end(), 0l);
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day13