//
// Created by alexoxorn on 2021-12-09.
//

#include "day08.h"
#include <sstream>
#include <set>
#include <algorithm>
#include <numeric>
#include <map>
#include <cassert>

#define YEAR 2021
#define DAY 08

namespace day08 {
    class signals {
        using lights = std::set<char>;
        struct custom_set_comp {
            bool operator()(const lights& a, const lights& b) const {
                if (a.size() != b.size())
                    return a.size() < b.size();
                return a < b;
            }
        };

        std::map<lights, int, custom_set_comp> part1;
        std::vector<std::pair<lights, int>> part0;
        std::vector<lights> part2;

        std::array<const lights*, 10> ns;
        std::array<char, 7> character_map;
    public:
        void reset() {
            ns = {};
            character_map = {};
            part1 = {};
            part2.clear();
        }

        friend std::istream& operator>>(std::istream& in, signals& self) {
            self.reset();
            std::string line;
            getline(in, line);
            std::istringstream ss(line);

            std::string part;
            while (ss >> part && part != "|") {
                self.part1[std::set(part.begin(), part.end())] = -1;
            }
            while(ss >> part) {
                self.part2.emplace_back(part.begin(), part.end());
            }
            return in;
        }

        long count_2347_outputs() {
            return stdr::count_if(part2, [](const lights& s) {
                auto x = {2, 3, 4, 7};
                return stdr::find(x, s.size()) != x.end();
            });
        };

        static lights sub(const lights& a, const lights& b) {
            lights out;
            stdr::set_difference(a, b, std::inserter(out, out.begin()));
            return out;
        }
        static lights sub2(const lights& a, const std::pair<lights, int>& b) { return sub(a, b.first); }

        static lights add(const lights& a, const lights& b) {
            lights out;
            stdr::set_union(a, b, std::inserter(out, out.begin()));
            return out;
        }
        static lights add2(const lights& a, const std::pair<lights, int>& b) { return add(a, b.first); }

        static lights mul(const lights& a, const lights& b) {
            lights out;
            stdr::set_intersection(a, b, std::inserter(out, out.begin()));
            return out;
        }
        static lights mul2(const lights& a, const std::pair<lights, int>& b) { return mul(a, b.first); }

        void solve() {
            auto temp = part1.begin();
            auto size2 = part1.begin();
            auto size3 = std::find_if(size2, part1.end(), [](auto x) {return x.first.size() == 3;});
            auto size4 = std::find_if(size3, part1.end(), [](auto x) {return x.first.size() == 4;});
            auto size5 = std::find_if(size4, part1.end(), [](auto x) {return x.first.size() == 5;});
            auto size6 = std::find_if(size5, part1.end(), [](auto x) {return x.first.size() == 6;});
            auto size7 = std::find_if(size5, part1.end(), [](auto x) {return x.first.size() == 7;});

            size2->second = 1; ns.at(1) = &size2->first;
            size3->second = 7; ns.at(7) = &size3->first;
            size4->second = 4; ns.at(4) = &size4->first;
            size7->second = 8; ns.at(8) = &size7->first;

            const lights full_set = {'a', 'b', 'c', 'd', 'e', 'f', 'e', 'g'};
            lights set_temp1;
            lights set_temp2;
            lights set_temp3;

            // Determining 'a'
            set_temp1 = sub(*ns[7], *ns[1]);
            assert(set_temp1.size() == 1);
            character_map.at(0) = *(set_temp1.begin());

            // Determining 'd'
            set_temp1 = std::accumulate(size5, size6, full_set, mul2);
            set_temp2 = std::accumulate(size6, size7, full_set, mul2);
            set_temp3 = sub(set_temp1, set_temp2);
            assert(set_temp3.size() == 1);
            character_map.at('d' - 'a') = *(set_temp3.begin());

            // Determining 0
            temp = std::find_if(size6, size7, [this](auto x) { return !x.first.contains(character_map.at('d' - 'a')); });
            assert(temp != size7);
            temp->second = 0; ns.at(0) = &temp->first;

            // Determining 'b'
            set_temp1 = sub(*ns[4], *ns[1]);
            set_temp2 = sub(set_temp1, {character_map.at('d' - 'a')});
            assert(set_temp2.size() == 1);
            character_map.at('b' - 'a') = *(set_temp2.begin());

            // Determining '5'
            temp = std::find_if(size5, size6, [this](auto x) { return x.first.contains(character_map.at('b' - 'a')); });
            assert(temp != size6);
            temp->second = 5; ns.at(5) = &temp->first;

            // Determining '6'
            temp = std::find_if(size6, size7, [this](auto x) {
                return mul(x.first, *ns[1]).size() == 1;
            });
            assert(temp != size7);
            temp->second = 6; ns.at(6) = &temp->first;

            // Determining '9'
            temp = std::find_if(size6, size7, [this](auto x) { return x.second == -1; });
            assert(temp != size7);
            temp->second = 9; ns.at(9) = &temp->first;

            // Determining '3'
            temp = std::find_if(size5, size6, [this](auto x) { return mul(x.first, *ns[7]) == *ns[7]; });
            assert(temp != size6);
            temp->second = 3; ns.at(3) = &temp->first;

            // Determining '2'
            temp = std::find_if(size5, size6, [this](auto x) { return x.second == -1; });
            assert(temp != size6);
            temp->second = 2; ns.at(2) = &temp->first;
        }

        int get_number() {
            int sum = 0;
            int base = 1;
            for (const auto& s : stdv::reverse(part2)) {
                sum += base * part1[s];
                base *= 10;
            }
            assert(sum > 0);
            return sum;
        }

        int solve_and_get_number() {
            solve();
            return get_number();
        }

        void print_parts() const {
            for (auto& s : part1) {
                for (auto& c : s.first)
                    printf("%c", c);
                printf(" ");
            }
            printf("| ");
            for (auto& s : part2) {
                for (auto& c : s)
                    printf("%c", c);
                printf(" ");
            }
            printf("\n");
        }
    };


    void puzzle1() {
        auto input = GET_STREAM(input, signals);
        auto counts = input | stdv::transform([](auto x){return x.count_2347_outputs();});
        auto x = std::accumulate(counts.begin(), counts.end(), 0l);
        printf("%ld\n", x);
    }

    void puzzle2() {
        auto input = GET_STREAM(input, signals);
        auto sums = input | stdv::transform([](auto x){return x.solve_and_get_number();});
        auto x = std::accumulate(sums.begin(), sums.end(), 0);
        printf("%d\n", x);
    }
}