// day6
//  Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <ox/utils.h>
#include <ox/future/vector_format.h>
#include <ox/combinators.h>
#include <variant>
#include <vector>
#include <stack>
#include <numeric>
#include <format>

namespace aoc2022::day13 {
    struct element;
    struct element_comparison_overload;
    using list = std::vector<element>;

    struct element {
        using datatype = std::variant<int, list>;
        datatype data;

        bool operator==(const element&) const = default;
    };
} // namespace aoc2022::day13

template <>
struct std::formatter<aoc2022::day13::element, char> {
    constexpr static auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    FormatContext::iterator format(const aoc2022::day13::element& obj, FormatContext& ctx) const {
        return std::visit([&ctx](const auto& e) { return std::format_to(ctx.out(), "{}", e); }, obj.data);
    }
};

namespace aoc2022::day13 {
    std::weak_ordering operator<=>(const element&, const element&);
    std::ostream& operator<<(std::ostream& out, [[maybe_unused]] const element& elm);

    struct element_comparison_overload {
        auto operator()(int l, int r) -> std::weak_ordering { return l <=> r; }
        auto operator()(const list& l, const list& r) {
            return std::lexicographical_compare_three_way(l.begin(), l.end(), r.begin(), r.end());
        }
        auto operator()(const list& l, int r) { return (*this)(l, std::vector{element{r}}); }
        auto operator()(int l, const list& r) { return (*this)(std::vector{element{l}}, r); }
    };

    std::weak_ordering operator<=>(const element& lhs, const element& rhs) {
        return std::visit(element_comparison_overload{}, lhs.data, rhs.data);
    }

    STREAM_IN(element, elm) {
        std::string s;
        while (std::getline(in, s) && s.empty())
            ;
        if (!in)
            return in;

        std::stack<list> element_stack;
        element_stack.emplace();
        list top;

        for (char* pointer = s.data(); *pointer != '\0'; pointer++) {
            if (*pointer == '[') {
                element_stack.emplace();
                continue;
            }

            if (*pointer == ']') {
                top = std::move(element_stack.top());
                element_stack.pop();
                element_stack.top().emplace_back(std::move(top));
                continue;
            }

            if (*pointer == ',')
                continue;

            element_stack.top().emplace_back(static_cast<int>(strtol(pointer, &pointer, 10)));
            pointer--;
        }

        assert(element_stack.size() == 1ul);
        assert(element_stack.top().size() == 1ul);
        elm = std::move(element_stack.top().back());

        return in;
    }

    STREAM_OUT(element, elm) {
        return out << std::format("{}", elm);
    }

    std::istream& operator>>(std::istream& in, std::pair<element, element>& elm_pair) {
        in >> elm_pair.first;
        in >> elm_pair.second;
        if (!in.eof())
            in.ignore(10000000, '\n');
        return in;
    }

    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<std::pair<element, element>>(filename);
        long i = 0;
        long sum = 0;
        for (const auto& [x, y] : input) {
            sum += (x < y) * ++i;
        }
        myprintf("Sum of the index of the properly ordered pairs is %ld\n", sum);
        return sum;
    }

    answertype puzzle2(puzzle_options filename) {
        list input = get_from_input<element>(filename);
        const element two(list{element{list{element{2}}}});
        const element six(list{element{list{element{6}}}});

        input.emplace_back(two);
        input.emplace_back(six);
        stdr::sort(input);

        long two_pos = stdr::find(input, two) - input.begin() + 1;
        long six_pos = stdr::find(input, six) - input.begin() + 1;

        myprintf("the product of indices of the two divider packets is %ld\n", two_pos * six_pos);
        return two_pos * six_pos;
    }
} // namespace aoc2022::day13