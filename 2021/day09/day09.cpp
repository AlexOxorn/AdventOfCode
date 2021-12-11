//
// Created by alexoxorn on 2021-12-10.
//

#include "day09.h"
#include <vector>
#include <numeric>
#include <queue>
#include <set>
#include <ranges>

#define YEAR 2021
#define DAY 09

namespace day09 {

    class heightmap {
        std::vector<int> data;
        int width = 0;
    public:
        std::pair<int, int> get_dimensions() {
            return {width, data.size() / width};
        }

        heightmap(std::istream& in) {
            std::string s;
            std::getline(in, s);
            width = s.size();
            std::transform(s.begin(), s.end(), std::back_inserter(data), [](char a) {return a - '0';});
            while(std::getline(in, s)) {
                std::transform(s.begin(), s.end(), std::back_inserter(data), [](char a) {return a - '0';});
            }
        }

        bool is_low_point(int index) {
            int value = data.at(index);
            if (index % width != 0 && data.at(index - 1) <= value)
                return false;
            if ((index + 1) % width && data.at(index + 1) <= value)
                return false;
            if (index - width >= 0 && data.at(index - width) <= value)
                return false;
            if (index + width <= data.size() && data.at(index + width) <= value)
                return false;
            return true;
        }

        int get_basin_size(int index) {
            std::set<int> indices_in_basin{index};
            std::queue<int> dfs;
            dfs.push(index);
            while(!dfs.empty()) {
                int current = dfs.front();
                dfs.pop();
                indices_in_basin.insert(current);
                if (int n = current-1; current % width != 0 && data.at(n) != 9)
                    if (!indices_in_basin.contains(n)) dfs.push(n);
                if (int n = current+1; n % width && data.at(n) != 9)
                    if (!indices_in_basin.contains(n)) dfs.push(n);
                if (int n = current-width; n >= 0 && data.at(n) != 9)
                    if (!indices_in_basin.contains(n)) dfs.push(n);
                if (int n = current+width; n < data.size() && data.at(n) != 9)
                    if (!indices_in_basin.contains(n)) dfs.push(n);
            }
            return indices_in_basin.size();
        }

        int get_score() {
            auto [w, h] = get_dimensions();
            auto x = stdv::iota(0, w * h)
                     | stdv::filter([this] (int x) { return is_low_point(x); })
                     | stdv::transform([this] (int x) { return data.at(x) + 1; });
            return std::accumulate(x.begin(), x.end(), 0);
        }

        int get_score2() {
            auto [w, h] = get_dimensions();
            auto x = stdv::iota(0, w * h)
                     | stdv::filter([this] (int x) { return is_low_point(x); })
                     | stdv::transform([this] (int x) { return get_basin_size(x); });
            std::vector basin_sizes(x.begin(), x.end());
            std::nth_element(basin_sizes.begin(), basin_sizes.begin() + 3, basin_sizes.end(), std::greater<int>{});

            return std::accumulate(basin_sizes.begin(), basin_sizes.begin() + 3, 1, std::multiplies<int>());
        }

        void print_array() {
            auto [x, y] = get_dimensions();
            for (int i : stdv::iota(0, y)) {
                for (int j : stdv::iota(0, x)) {
                    if (is_low_point(i * width + j)) {
                        printf("\033[31m%1d\033[0m", data.at(i * width + j));
                    } else if (data.at(i * width + j) == 9) {
                        printf("\033[30m%1d\033[0m", data.at(i * width + j));
                    } else {
                        printf("%1d", data.at(i * width + j));
                    }
                }
                printf("\n");
            }
        }
    };

    void puzzle1() {
        auto input = GET_STREAM(input, int);
        heightmap h(input);
        auto [x, y] = h.get_dimensions();
        printf("size = %d x %d\n", x, y);
        h.print_array();
        printf("score = %d\n", h.get_score());
    }

    void puzzle2() {
        auto input = GET_STREAM(input, int);
        heightmap h(input);
        printf("score = %d\n", h.get_score2());
    }
}