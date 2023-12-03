#include "../../../common.h"
#include <ox/grid.h>
#include <cctype>
#include <numeric>
#include <regex>

namespace aoc2023::day03 {
    using gridtype = std::pair<char, bool>;

    struct blueprint : ox::grid<gridtype> {
        using ox::grid<gridtype>::grid;
        using ox::grid<gridtype>::raw_iterator;

        static bool is_symbol(char c) {
            if (isdigit(c))
                return false;
            return c != '.';
        }

        void mark_neighbours(raw_iterator head, bool b = true) {
            head->second = b;
            auto neighbours = neighbour_range(head);
            auto values = neighbours | stdv::filter([](const auto& x) { return x.has_value(); })
                        | stdv::transform([](const auto& x) { return *x; });
            stdr::for_each(values, [b](const raw_iterator& r) { r->second = b; });
        }

        void mark() {
            auto head = this->data.begin();
            const auto end = this->data.end();
            for (; head < end; ++head) {
                if (is_symbol(head->first)) {
                    mark_neighbours(head);
                }
            }
        }

        void unmark() {
            stdr::for_each(data, [](gridtype& g) { g.second = false; });
        }

        template <typename F>
        std::pair<long, long> read_marked_numbers(long def, F op) {
            long count = 0;
            long sum = def;
            for (const auto& row : *this) {
                long temp = 0;
                bool marked = false;
                for (auto [c, m] : row) {
                    if (m && isdigit(c)) {
                        marked = true;
                    }
                    if (isdigit(c)) {
                        temp *= 10;
                        temp += c - '0';
                    }
                    if (!isdigit(c)) {
                        sum = marked ? op(sum, temp) : op(sum, def);
                        count += marked;
                        temp = 0;
                        marked = false;
                    }
                }
                sum = marked ? op(sum, temp) : op(sum, def);
                count += marked;
            }
            return {sum, count};
        }

        long solution() {
            mark();
            auto [sum, count] = read_marked_numbers(0, std::plus<>());
            unmark();
            return sum;
        }

        long solution2() {
            auto head = this->data.begin();
            const auto end = this->data.end();
            long sum = 0;
            for (; head < end; ++head) {
                if (head->first == '*') {
                    mark_neighbours(head);
                    auto [ratio, count] = read_marked_numbers(1, std::multiplies<>());
                    sum += (count == 2) * ratio;
                    mark_neighbours(head, false);
                }
            }
            return sum;
        }

        void print_array() {
            leveled_foreach(
                    [](const gridtype& elem) {
                        myprintf("\033[%sm%c\033[0m", elem.second ? "31" : "0", elem.first);
                    },
                    []() { myprintf("\n"); });
            myprintf("\n");
        }
    };

    blueprint& get_blueprint(puzzle_options filename) {
        static blueprint b(get_stream<ox::line>(filename), [](char c) -> gridtype { return {c, false}; });
        return b;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        blueprint& b = get_blueprint(filename);
        long res = b.solution();
        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        blueprint& b = get_blueprint(filename);
        long res = b.solution2();
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day03