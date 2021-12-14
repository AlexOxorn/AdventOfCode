//
// Created by alexoxorn on 2021-12-13.
//

#include "day13.h"

#define YEAR 2021
#define DAY 13

#include <vector>
#include <unordered_set>
#include <set>
#include <queue>
#include <numeric>
#include <optional>
#include <concepts>
#include <algorithm>

namespace day13 {
    struct fold : public std::pair<char, int> {};

    struct paper {
        using point = std::pair<int, int>;
        std::set<point> points;
        point dimensions;

        void foldY(int cress) {
            std::set<point> new_set;
            for (point p : points) {
                if (p.second < cress)
                    new_set.insert(p);
                if (p.second > cress)
                    new_set.insert({p.first, 2 * cress - p.second});
            }
            points = new_set;
            dimensions.second = cress;
        }

        void foldX(int cress) {
            std::set<point> new_set;
            for (point p : points) {
                if (p.first < cress)
                    new_set.insert(p);
                if (p.first > cress)
                    new_set.insert({2 * cress - p.first, p.second});
            }
            points = new_set;
            dimensions.first = cress;
        }
    public:
        explicit paper(std::istream& in) {
            std::string s;
            while(std::getline(in, s) && !s.empty()) {
                point p;
                std::sscanf(s.c_str(), "%d,%d", &p.first, &p.second);
                points.insert(p);
            }
            dimensions.first = stdr::max(points | stdv::transform(&point::first)) + 1;
            dimensions.second = stdr::max(points | stdv::transform(&point::second)) + 1;
        }

        void print_paper(int limit = 100) {
            for(int j : stdv::iota(0, std::min(dimensions.second, limit))) {
                for (int i : stdv::iota(0,  std::min(dimensions.first, limit))) {
                    printf("%s", points.contains({i, j}) ? "\033[41m \033[0m" : " ");
                }
                printf("\n");
            }
        }

        void fold(fold f) {
            if (f.first == 'y')
                foldY(f.second);
            else
                foldX(f.second);
        }

        auto point_count() {
            return points.size();
        }
    };

    std::istream& operator>>(std::istream& in, fold& f) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "fold along %c=%d", &f.first, &f.second);
        return in;
    }

    void puzzle1() {
        auto input = GET_STREAM(input, fold);
        paper p(input);
        p.fold(*input.begin());
        printf("The number of points after one folder are %zu\n", p.point_count());
    }

    void puzzle2() {
        auto input = GET_STREAM(input, fold);
        paper p(input);
        for (auto f : input) {
            p.fold(f);
        }
        p.print_paper();
    }
}
