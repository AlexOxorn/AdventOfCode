//
// Created by alexoxorn on 2021-12-20.
//

#include "day18.h"
#include <ox/tree.h>
#include <algorithm>
#include <cassert>
#include <ranges>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <ox/debug.h>
#include <ox/ranges.h>

#define YEAR 2021
#define DAY 18

namespace day18 {
    using snail_number_node = ox::binary_tree_node<std::optional<int>>;
    using snail_number = ox::binary_tree<std::optional<int>>;

    snail_number_node read_snail_number(std::istream& in) {
        snail_number_node to_return;
        int c = in.peek();
        assert(c == '[' || ('0' <= c && c <= '9'));
        if (c == '[') {
            in.get();
            to_return.emplace_left(read_snail_number(in));
            in.get();
            to_return.emplace_right(read_snail_number(in));
            in.get();
        } else if ('0' <= c && c <= '9') {
            int i;
            in >> i;
            to_return.value = i;
        }
        return to_return;
    }

    snail_number read_snail_number(const std::string& s) {
        std::stringstream ss(s);
        return ox::binary_tree(read_snail_number(ss));
    }

    snail_number add(const snail_number& a, const snail_number& b) {
        snail_number_node to_return;
        to_return.emplace_left(a.head);
        to_return.emplace_right(b.head);
        return ox::binary_tree(std::move(to_return));
    }

    int magnitude(const snail_number_node& node) {
        if(node.value) {
            return *node.value;
        }
        return 3 * magnitude(node.get_left_child()) + 2 * magnitude(node.get_right_child());
    }

    void print_number(const snail_number_node& node) {
        if (node.value) {
            printf("%d", *node.value);
        } else {
            printf("[");
            print_number(node.get_left_child());
            printf(",");
            print_number(node.get_right_child());
            printf("]");
        }
    }

    void explode(snail_number& source, snail_number::iterator d) {
        auto& current = *d;
        auto next = std::find_if(
               std::next(snail_number::iterator(current.get_right_child())),
               source.end(), [](const auto& x) { return x.value.has_value(); });
        auto prev = std::find_if(
               snail_number::reverse_iterator(snail_number::iterator(current.get_left_child())),
               source.rend(), [](const auto& x) { return x.value.has_value(); });
        if (prev != source.rend())
            *prev->value += *current.get_left_child().value;
        if (next != source.end())
            *next->value += *current.get_right_child().value;
        current.value = 0;
        current.children.first.reset();
        current.children.second.reset();
    }
    bool explode(snail_number& b) {
        auto issue_finder = b | stdv::filter([](const auto& x) {
            return !x.value && x.get_depth() >= 4;
        });
        auto current_iter = issue_finder.begin().base();
        if (current_iter != b.end()) {
            explode(b, current_iter);
            return true;
        }
        return false;
    }

    void split(snail_number::iterator d) {
        auto& current = *d;
        int old_value = *current.value;
        current.emplace_left(old_value/2);
        current.emplace_right(old_value/2 + old_value%2);
        current.value.reset();
    }
    bool split(snail_number& b) {
        auto issue_finder2 = b | stdv::filter([](const auto& x) {
            return x.value && *x.value >= 10;
        });
        auto current_iter = issue_finder2.begin().base();
        if (current_iter != b.end()) {
            split(current_iter);
            return true;
        }
        return false;
    }

    bool fix(snail_number& b) {
        return explode(b) || split(b);
    }

    void puzzle1() {
        const int verbose_level = 0;
        auto input = GET_STREAM(input, ox::line);
        std::string s = *input.begin();
        auto y = read_snail_number(s);
        for(const auto& line : input) {
            auto z = read_snail_number(line);

            if (verbose_level >= 1) {
                printf("  "); print_number(y.head);
                printf("\n+ "); print_number(z.head);
            }
            y = add(y, z);
            if (verbose_level >= 2) {
                printf("\n= "); print_number(y.head);
            }

            while(fix(y)) {
                if(verbose_level >= 3) {
                    printf("\n= ");
                    print_number(y.head);
                }
            }
            if (verbose_level >= 1) {
                printf("\n= ");
                print_number(y.head);
                printf("\n\n");
            }
        }
        printf("final magnitude is equal to %d\n", magnitude(y.head));
    }

    void puzzle2() {
        auto input = GET_STREAM(input, ox::line);
        int max = 0;
        std::vector<snail_number> numbers;
        stdr::transform(input, std::back_inserter(numbers), [](ox::line l) { return read_snail_number(l); });
        for(const auto& left : numbers) {
            for (const auto& right : numbers) {
                if (&left == &right)
                    continue;
                auto y = add(left, right);
                while(fix(y));
                max = std::max(max, magnitude(y.head));
            }
        }
        printf("largest magnitude is equal to %d\n", max);
    }
} // namespace day18