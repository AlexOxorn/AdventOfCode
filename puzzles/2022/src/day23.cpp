//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <set>
#include <map>
#include <algorithm>
#include <thread>
#include <queue>
#include <numeric>

namespace aoc2022::day23 {
    struct elf;
    using elf_list = std::vector<elf>;
    using markers = std::map<std::pair<int, int>, int>;
    using position_predicate = bool (*)(const elf_list&, int, int);
    
    bool contains_elf(const elf_list& elves, int x, int y);
    bool check_all(const elf_list& elf_positions, int x, int y);
    bool check_north(const elf_list& elf_positions, int x, int y);
    bool check_south(const elf_list& elf_positions, int x, int y);
    bool check_west(const elf_list& elf_positions, int x, int y);
    bool check_east(const elf_list& elf_positions, int x, int y);
    
    position_predicate predicates[] = {check_north, check_south, check_west, check_east};
    std::pair<int, int> move(int x, int y, int dir_index) {
        switch (dir_index) {
            case 0: return {x, y - 1};
            case 1: return {x, y + 1};
            case 2: return {x - 1, y};
            case 3: return {x + 1, y};
            default: return {x, y};
        }
    }

    char movechar(int dir_index) {
        switch (dir_index) {
            case 0: return '^';
            case 1: return 'v';
            case 2: return '<';
            case 3: return '>';
            default: return '#';
        }
    }
    
    struct elf {
        int x = 0;
        int y = 0;
        mutable int dir = -1;

        auto operator <=>(const elf& other) const {
            return std::pair(y, x) <=> std::pair(other.y, other.x);
        }
        bool operator ==(const elf& other) const {
            return std::pair(x, y) == std::pair(other.x, other.y);
        }
        
        void step1(const elf_list& current, markers& marked, int start) const {
            if (check_all(current, x, y)) {
                dir = -1;
                return;
            }
            for (int i = 0; i < 4; i++) {
                if (predicates[i](current, x, y)) {
                    dir = (start + i) % 4;
                    marked[move(x, y, dir)]++;
                    return;
                }
            }
            dir = -1;
        }

        void step2(const markers& marked) {
            auto newposition = move(x, y, dir);
            if (dir >= 0 && marked.at(newposition) == 1) {
                auto [new_x, new_y] = newposition;
                x = new_x;
                y = new_y;
            }
        }
    };

    bool contains_elf(const elf_list& elves, int x, int y) {
        return stdr::binary_search(elves, elf{x, y});
    }

    bool check_all(const elf_list& elf_positions, int x, int y) {
        if (!check_north(elf_positions, x, y))
            return false;
        if (!check_south(elf_positions, x, y))
            return false;
        if (!check_west(elf_positions, x, y))
            return false;
        if (!check_east(elf_positions, x, y))
            return false;
        return true;
    }

    bool check_north(const elf_list& elf_positions, int x, int y) {
        if (contains_elf(elf_positions, x - 1, y - 1))
            return false;
        if (contains_elf(elf_positions, x, y - 1))
            return false;
        if (contains_elf(elf_positions, x + 1, y - 1))
            return false;
        return true;
    }
    bool check_south(const elf_list& elf_positions, int x, int y) {
        if (contains_elf(elf_positions, x - 1, y + 1))
            return false;
        if (contains_elf(elf_positions, x, y + 1))
            return false;
        if (contains_elf(elf_positions, x + 1, y + 1))
            return false;
        return true;
    }
    bool check_west(const elf_list& elf_positions, int x, int y) {
        if (contains_elf(elf_positions, x - 1, y + 1))
            return false;
        if (contains_elf(elf_positions, x - 1, y))
            return false;
        if (contains_elf(elf_positions, x - 1, y - 1))
            return false;
        return true;
    }
    bool check_east(const elf_list& elf_positions, int x, int y) {
        if (contains_elf(elf_positions, x + 1, y + 1))
            return false;
        if (contains_elf(elf_positions, x + 1, y))
            return false;
        if (contains_elf(elf_positions, x + 1, y - 1))
            return false;
        return true;
    }

    void print_elves(const elf_list& elves) {
        auto [minx, maxx] = stdr::minmax(elves | stdv::transform(&elf::x));
        auto [miny, maxy] = stdr::minmax(elves | stdv::transform(&elf::y));
        auto iter = elves.begin();
        for (int y = miny; y <= maxy; y++) {
            for (int x = minx; x <= maxx; x++) {
                if (iter != elves.end() && iter->x == x && iter->y == y) {
                    putchar(movechar(iter->dir));
                    ++iter;
                } else {
                    putchar('.');
                }
            }
            putchar('\n');
        }
        myprintf("=============================\n");
    }

    elf_list& create_elves(const char* filename) {
        static elf_list elves;
        if (elves.empty())
            stdr::for_each(get_stream<ox::line>(filename), [y=0] (const std::string& s) mutable {
                for (int i = 0; i < static_cast<int>(s.length()); i++) {
                    if (s[i] == '#') {
                        elves.push_back({i, y});
                    }
                }
                y++;
            });
        return elves;
    }

    answertype puzzle1(const char* filename) {
        elf_list& elves = create_elves(filename);

        for (int i = 0; i < 10; i++) {
            markers m;
            for (const auto& x : elves) {
                x.step1(elves, m, i%4);
            }
            for (auto& x : elves) {
                x.step2(m);
            }
            stdr::rotate(predicates, std::begin(predicates) + 1);
            stdr::sort(elves);
        }

        auto [minx, maxx] = stdr::minmax(elves | stdv::transform(&elf::x));
        auto [miny, maxy] = stdr::minmax(elves | stdv::transform(&elf::y));
        size_t empty_spaces = (maxx - minx + 1) * (maxy - miny + 1) - elves.size();
        myprintf("Number of empty spaces = %zu\n", empty_spaces);
        return empty_spaces;
    }

    answertype puzzle2(const char* filename) {
        elf_list& elves = create_elves(filename);

        int index = *stdr::find_if(stdv::iota(10), [&elves](int i) {
            markers m;
            for (const auto& x : elves) {
                x.step1(elves, m, i%4);
            }
            if (stdr::none_of(m | stdv::values, [](int i) { return i == 1; })) {
                return true;
            }

            for (auto& x : elves) {
                x.step2(m);
            }
            stdr::rotate(predicates, std::begin(predicates) + 1);
            stdr::sort(elves);
            return false;
        });

        myprintf("Number of empty spaces before no movement is %d\n", index + 1);
        return index + 1;
    }
} // namespace aoc2022::day23