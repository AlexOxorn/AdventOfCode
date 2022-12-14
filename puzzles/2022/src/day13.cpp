//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <variant>
#include <vector>
#include <stack>
#include <numeric>

namespace aoc2022::day13 {
    struct element;
    struct element_comparison_overload;
    using list = std::vector<element>;

    struct element : std::variant<int, list> {
        using std::variant<int, list>::variant;
    };

    bool operator<(const element&, const element&);

    struct element_comparison_overload {
        auto operator()(int l, int r) { return l < r; }
        auto operator()(const list& l, const list& r) { return stdr::lexicographical_compare(l, r); }
        auto operator()(const list& l, int r) { return (*this)(l, std::vector{element{r}}); }
        auto operator()(int l, const list& r) { return (*this)(std::vector{element{l}}, r); }
    };
    struct element_output_overload {
        void operator()(int l) { std::cout << l; }
        void operator()(const list& l) {
            std::cout << '[';
            for (const element& x : l) {
                std::visit(element_output_overload{}, x);
                if (&x != &l.back())
                    std::cout << ',';
            }
            std::cout << ']';
        };
    };

    bool operator<(const element& lhs, const element& rhs) {
        return std::visit(element_comparison_overload{}, lhs, rhs);
    }

    std::istream& operator>>(std::istream& in, element& elm) {
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

    std::istream& operator>>(std::istream& in, std::pair<element, element>& elm_pair) {
        in >> elm_pair.first;
        in >> elm_pair.second;
        if (!in.eof())
            in.ignore(10000000, '\n');
        return in;
    }

    void puzzle1(const char* filename) {
        auto input = get_stream<std::pair<element, element>>(filename);
        auto valid_indices = input | stdv::transform([index = 0](const auto& x) mutable {
                                 ++index;
                                 return x.first < x.second ? index : 0;
                             });
        long sum = std::accumulate(valid_indices.begin(), valid_indices.end(), 0l);
        printf("Sum of the index of the properly ordered pairs is %ld\n", sum);
    }

    void puzzle2(const char* filename) {
        list input = get_from_input<element>(filename);
        element two(list{element{list{element{2}}}});
        element six(list{element{list{element{6}}}});

        input.emplace_back(two);
        input.emplace_back(six);
        stdr::sort(input);

        long two_pos = stdr::find(input, two) - input.begin() + 1;
        long six_pos = stdr::find(input, six) - input.begin() + 1;

        printf("the product of indices of the two divider packets is %ld\n", two_pos * six_pos);
    }
} // namespace aoc2022::day13