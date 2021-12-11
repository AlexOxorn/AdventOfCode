//
// Created by alexoxorn on 2021-12-11.
//

#include "day11.h"

#define YEAR 2021
#define DAY 11

#include <vector>
#include <set>
#include <queue>
#include <numeric>
#include <optional>
#include <concepts>

namespace day11 {
    class octopuses {
        std::vector<int> data;
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

        static auto valid_index() {
            return stdv::filter([](std::optional<int> x) { return x.has_value(); } )
                | stdv::transform([] (std::optional<int> x) { return *x; });
        }
    public:
        auto get_size() {
            return data.size();
        }

        std::pair<int, int> get_dimensions() {
            return {width, data.size() / width};
        }

        explicit octopuses(std::istream &in) {
            std::string s;
            while (std::getline(in, s)) {
                width = s.size();
                std::transform(s.begin(), s.end(), std::back_inserter(data), [](char a) { return a - '0'; });
            }
        }

        int next_step() {
            std::set<int> indices_flashed{};
            std::queue<int> dfs;

            for(auto& x : data) {
                if(++x > 9) {
                    dfs.push(&x - &data.front());
                }
            }

            while (!dfs.empty()) {
                int current = dfs.front();
                dfs.pop();
                auto [in_iter, success] = indices_flashed.insert(current);
                if (!success) {
                    continue;
                }

                auto neighbour = neighbour_range(current);
                for(auto index : neighbour | valid_index()) {
                    data.at(index)++;
                    if (data.at(index) > 9)
                        dfs.push(index);
                }
            }

            for (auto x : indices_flashed) {
                data.at(x) = 0;
            }

            return indices_flashed.size();
        }

        void print_array() {
            auto[x, y] = get_dimensions();
            for (int i: stdv::iota(0, y)) {
                for (int j: stdv::iota(0, x)) {
                    if (data.at(i * width + j) == 0 || data.at(i * width + j) > 9) {
                        printf("\033[31m%4d\033[0m", data.at(i * width + j));
                    } else {
                        printf("%4d", data.at(i * width + j));
                    }
                }
                printf("\n");
            }
        }
    };

    void puzzle1() {
        int number_of_steps = 100;
        auto input = GET_STREAM(input, int);
        octopuses o(input);
        auto scores = stdv::iota(0, number_of_steps) | stdv::transform([&o] (int a) { return o.next_step(); });
        printf("Number of flashes after %d steps: %d\n", number_of_steps, std::accumulate(scores.begin(), scores.end(), 0));
    }

    void puzzle2() {
        auto input = GET_STREAM(input, int);
        octopuses o(input);
        auto scores = stdv::iota(1)
                | stdv::transform([&o] (int a) { return std::make_pair(a, o.next_step()); })
                | stdv::filter([&o](std::pair<int, int> a) { return a.second == o.get_size(); })
                | stdv::take(1);
        printf("first point when all octopuses flash: %d\n", (*scores.begin()).first);
    }
}
