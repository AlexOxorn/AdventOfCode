//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <variant>
#include <numeric>
#include <cstring>
#include <cassert>
#include <ox/tree.h>

namespace aoc2022::day21 {
    template <class... Ts>
    struct overloaded : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    std::function<long(long, long)> from_op(char c) {
        switch (c) {
            case '+': return std::plus<>();
            case '-': return std::minus<>();
            case '*': return std::multiplies<>();
            case '/': return std::divides<>();
            default: throw std::exception();
        }
    }

    char inverse_op(char c) {
        switch (c) {
            case '+': return '-';
            case '-': return '+';
            case '*': return '/';
            case '/': return '*';
            default: throw std::exception();
        }
    }

    struct math_monkey {
        std::string monkey1, monkey2;
        char op;
    };

    using monkey = std::variant<long, math_monkey>;
    using monkey_map = std::unordered_map<std::string, monkey>;
    static monkey_map map;

    long get_value(const std::string& name) {
        return std::visit(overloaded{[](const math_monkey& x) {
                                         return from_op(x.op)(get_value(x.monkey1), get_value(x.monkey2));
                                     },
                                     [](long l) {
                                         return l;
                                     }},
                          map.at(name));
    }

    void register_monkey(const std::string& s) {
        char name[5];
        char sub1[5];
        char sub2[5];
        char op;
        int val;
        if (sscanf(s.data(), "%4s: %4s %c %4s", name, sub1, &op, sub2) == 4) {
            map[name] = math_monkey{sub1, sub2, op};
        } else if (sscanf(s.data(), "%4s: %d", name, &val) == 2) {
            map[name] = val;
        } else {
            throw std::exception();
        }
    }

    struct human {};
    using node_type = std::variant<long, char, human>;
    using math_tree = ox::binary_tree<node_type>;
    using math_tree_node = ox::binary_tree<node_type>::node;

    void get_tree_data(math_tree_node& tree, const std::string& name) {
        if (name == "humn") {
            tree.value = human{};
            return;
        }
        std::visit(overloaded{[&tree](long l) { tree.value = l; },
                              [&tree](const math_monkey& m) {
                                  tree.value = m.op;
                                  tree.emplace_left();
                                  tree.emplace_right();
                                  get_tree_data(*tree.children.first, m.monkey1);
                                  get_tree_data(*tree.children.second, m.monkey2);
                              }},
                   map[name]);
    }

    bool reduce(math_tree_node& tree) {
        if (std::holds_alternative<long>(tree.value)) {
            return true;
        }
        if (std::holds_alternative<human>(tree.value)) {
            return false;
        }

        bool left_reduce = reduce(*tree.children.first);
        bool right_reduce = reduce(*tree.children.second);

        if (!(left_reduce && right_reduce)) {
            return false;
        }

        tree.value = from_op(std::get<char>(tree.value))(std::get<long>(tree.children.first->value),
                                                         std::get<long>(tree.children.second->value));
        tree.children.first.reset();
        tree.children.second.reset();
        return true;
    }

    void print_tree(math_tree_node& tree) {
        if (tree.children.first) {
            myprintf("(");
            print_tree(*tree.children.first);
        }
        std::visit(overloaded{[](long l) { myprintf("%ld", l); },
                              [](char c) { myprintf("%c", c); },
                              [](human) {
                                  myprintf("x");
                              }},
                   tree.value);
        if (tree.children.second) {
            print_tree(*tree.children.second);
            myprintf(")");
        }
    }

    void print_equality(math_tree_node& lhs, math_tree_node& rhs, bool force = false) {
        if (map.size() >= 20 and not force)
            return;
        print_tree(lhs);
        myprintf(" == ");
        print_tree(rhs);
        myprintf("\n");
    }


    void move_x_to_left(math_tree_node& lhs, math_tree_node& rhs) {
        if (std::holds_alternative<long>(lhs.value)) {
            std::swap(lhs, rhs);
        }
    }

    void solve_part(math_tree_node& lhs, math_tree_node& rhs) {
        assert(std::holds_alternative<long>(rhs.value));
        assert(std::holds_alternative<char>(lhs.value));
        switch (std::get<char>(lhs.value)) {
            case '+':
            case '*': move_x_to_left(*lhs.children.first, *lhs.children.second); [[fallthrough]];
            default:
                rhs.emplace_left(rhs.value);
                rhs.emplace_right(std::move(*lhs.children.second));
                rhs.value = inverse_op(std::get<char>(lhs.value));
                lhs = math_tree_node{*lhs.children.first};
                break;
        }
    }


    answertype puzzle1(puzzle_options filename) {
        auto monkey_stream = get_stream<ox::line>(filename);
        stdr::for_each(monkey_stream, register_monkey);
        long root_value = get_value("root");
        myprintf("The 'root' monkey will yell %ld\n", root_value);
        return root_value;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        math_tree lhs_tree;
        math_tree rhs_tree;
        auto x = std::get<math_monkey>(map["root"]);

        get_tree_data(lhs_tree.head, x.monkey1);
        get_tree_data(rhs_tree.head, x.monkey2);

        auto& lhs = lhs_tree.head;
        auto& rhs = rhs_tree.head;

        move_x_to_left(lhs, rhs);
        print_equality(lhs, rhs);

        while (!std::holds_alternative<human>(lhs.value) && !std::holds_alternative<human>(rhs.value)) {
            reduce(lhs);
            reduce(rhs);
            print_equality(lhs, rhs);
            solve_part(lhs, rhs);
            move_x_to_left(lhs, rhs);
            print_equality(lhs, rhs);
        }

        reduce(rhs);
        print_equality(lhs, rhs, true);
        return std::get<long>(rhs.value);
    }
} // namespace aoc2022::day21