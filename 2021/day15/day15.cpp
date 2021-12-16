//
// Created by alexoxorn on 2021-12-15.
//

#include <cassert>
#include <set>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <limits>
#include <queue>
#include <cmath>
#include <numeric>
#include "day15.h"

#define YEAR 2021
#define DAY 15

namespace day15 {
    struct node {
        int value;
        int index;
        int g_cost = std::numeric_limits<int>::max();
        node* parent = nullptr;
        bool in_path = false;

        auto operator<=>(const node& other) const {
            return g_cost <=> other.g_cost;
        };
    };

    class grid {
        std::vector<node> data;
        int width = 0;

        [[nodiscard]] std::optional<int> up(std::optional<int> curr) const {
            if (!curr || *curr < width)
                return std::nullopt;
            return *curr - width;
        }
        [[nodiscard]] std::optional<int> left(std::optional<int> curr) const {
            if (!curr || *curr % width == 0)
                return std::nullopt;
            return *curr - 1;
        }
        [[nodiscard]] std::optional<int> right(std::optional<int> curr) const {
            if (!curr || (*curr + 1) % width == 0)
                return std::nullopt;
            return *curr + 1;
        }
        [[nodiscard]] std::optional<int> down(std::optional<int> curr) const {
            if (!curr || (*curr + width) >= data.size())
                return std::nullopt;
            return *curr + width;
        }

        auto neighbour_range(int current) {
            std::array neighbours{up(current), up(left(current)), left(current), down(left(current)),
                                  down(current), down(right(current)), right(current), up(right(current))};
            return neighbours;
        }
        auto cardinal_neighbour_range(int current) {
            std::array neighbours{up(current), left(current), down(current), right(current)};
            return neighbours;
        }
        static auto valid_index() {
            return stdv::filter([](std::optional<int> x) { return x.has_value(); } )
                   | stdv::transform([] (std::optional<int> x) { return *x; });
        }
        auto to_value() {
            return stdv::transform([this](int index) -> node& { return data.at(index); } );
        }

        [[nodiscard]] std::pair<int, int> coord_from_index(int index) const {
            return {index % width, index/width};
        }
    public:
        auto get_size() {
            return data.size();
        }

        std::pair<int, int> get_dimensions() {
            return {width, data.size() / width};
        }

        explicit grid(std::istream &in) {
            std::string s;
            int index = 0;
            auto node_creator = [&index](char a) {
                return node{a - '0', index++};
            };
            while (std::getline(in, s)) {
                width = s.size();
                std::transform(s.begin(), s.end(), std::back_inserter(data), node_creator);
            }
            data.front().g_cost = 0;
        }

        explicit grid(std::istream &in, int multiply) {
            std::string s;
            int index = 0;
            while (std::getline(in, s)) {
                width = s.size() * multiply;
                for(int offset : stdv::iota(0, multiply)) {
                    std::transform(s.begin(), s.end(), std::back_inserter(data), [&index, offset](char a) {
                        return node{(a - '0' + offset)%10 + (a - '0' + offset)/10, index++};
                    });
                }
            }
            data.reserve(multiply * data.size());
            auto initial_end = data.end();
            for(int offset : stdv::iota(1, multiply)) {
                std::transform(data.begin(), initial_end, std::back_inserter(data), [&index, offset](node a) {
                    return node{(a.value + offset)%10 + (a.value + offset)/10, index++};
                });
            }
            data.front().g_cost = 0;
        }

        std::vector<node*> retrace_path() {
            std::vector<node*> to_return;
            node* back = &data.back();
            back->in_path = true;
            to_return.push_back(back);
            while((back = to_return.back()->parent) != nullptr) {
                back->in_path = true;
                to_return.push_back(back);
            }
            stdr::reverse(to_return);
            return to_return;
        }

        std::vector<node*> find_path() {
            auto index_less_than = [](const node* a, const node* b) { return a->index < b->index; };
            std::vector<node*> open_set;
            std::set<node*> closed_set;
            open_set.push_back(&data.front());
            stdr::push_heap(open_set, std::greater<>());
            while (!open_set.empty()) {
                stdr::pop_heap(open_set, std::greater<>());
                node* curr = open_set.back();
                open_set.pop_back();
                closed_set.insert(curr);
                if (curr == &data.back())
                    break;

                auto neighbours = cardinal_neighbour_range(curr->index);
                for(node& neighbour : neighbours | valid_index() | to_value()) {

                    int new_move_cost = curr->g_cost + neighbour.value;
                    bool not_in_open = stdr::find(open_set, &neighbour) == open_set.end();
                    if (new_move_cost < neighbour.g_cost) {
                        neighbour.g_cost = new_move_cost;
                        neighbour.parent = curr;
                        if (not_in_open) {
                            open_set.push_back(&neighbour);
                            stdr::push_heap(open_set, std::greater<>());
                        } else {
                            auto not_heap = stdr::is_heap_until(open_set, std::greater<>());
                            if (not_heap != open_set.end())
                                std::push_heap(open_set.begin(), not_heap + 1, std::greater<>());
                        }
                    }
                }
            }
            return retrace_path();
        }

        void print_grid() {
            auto[x, y] = get_dimensions();
            for (int i: stdv::iota(0, y)) {
                for (int j: stdv::iota(0, x)) {
                    if (auto x = data.at(i * width + j); x.in_path) {
                        printf("\033[31m%-1d\033[0m", x.value);
                    } else {
                        printf("%-1d", x.value);
                    }
                }
                printf("\n");
            }
        }
    };

    void puzzle1() {
        auto input = GET_STREAM(input, int);
        grid g(input);
        auto x = g.find_path();
        g.print_grid();
        int risk = std::accumulate(x.begin() + 1, x.end(), 0, [](int sum, const node* n) { return sum + n->value; });
        printf("Total Risk = %d\n", risk);
    }

    void puzzle2() {
        auto input = GET_STREAM(input, int);
        grid g(input, 5);
        auto x = g.find_path();
        g.print_grid();
        int risk = std::accumulate(x.begin() + 1, x.end(), 0, [](int sum, const node* n) { return sum + n->value; });
        printf("Total Risk = %d\n", risk);
    }
}
