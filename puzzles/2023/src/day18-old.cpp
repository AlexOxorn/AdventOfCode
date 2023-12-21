#include "../../../common.h"
#include "pipes.h"

#include <set>
#include <charconv>
#include <algorithm>
#include <format>
#include <ox/formatting.h>
#include <ox/colors.h>
#include <ox/infinite_grid.h>
#include <ox/grid.h>

namespace aoc2023::day18 {
    struct dig_directions {
        uint32_t color;
        int amount;
        char dir;
        char dir2;
    };

    // 0 means R, 1 means D, 2 means L, and 3 means U.
    char to_dir(char c) {
        switch (c) {
            case '0': return 'R';
            case '1': return 'D';
            case '2': return 'L';
            case '3': return 'U';
            default: std::unreachable();
        }
    }

    // ==========================================
    // PARSING
    // ==========================================

    STREAM_IN(dig_directions, d) {
        std::string color;
        in >> d.dir >> d.amount >> color;
        if (!in)
            return in;
        std::from_chars(color.c_str() + 2, color.c_str() + 7, d.color, 16);
        d.dir2 = to_dir(color[7]);
        return in;
    }

    STREAM_OUT(dig_directions, d) {
        return out << ox::color(d.color) << d.dir << " " << d.amount << ox::format{ox::escape::reset} << std::endl;
    }

    // ==========================================
    // PART 1
    // ==========================================

    std::array<long, 2> move(std::array<long, 2> pos, char dir) {
        switch (dir) {
            case 'U': return {pos[0], pos[1] - 1};
            case 'L': return {pos[0] - 1, pos[1]};
            case 'D': return {pos[0], pos[1] + 1};
            case 'R': return {pos[0] + 1, pos[1]};
            default: std::unreachable();
        }
    }

    std::array<long, 2> find_first_inside(ox::grid<char>& g) {
        char* inside_pointer = nullptr;
        for (auto row : g) {
            bool inside = false;
            bool last_hash = false;
            for (auto& c : row) {
                if (c != '.') {
                    if (last_hash)
                        break;
                    inside = !inside;
                    last_hash = true;
                    continue;
                }
                last_hash = false;
                if (c == '.' && inside) {
                    inside_pointer = &c;
                }
            }
            if ((!inside || last_hash) && inside_pointer) {
                break;
            }
            inside_pointer = nullptr;
        }
        assert(inside_pointer);
        long index = inside_pointer - g.get_raw().begin().base();
        return g.coord_from_index(index);
    }

    void bfs_inside(ox::grid<char>& g, std::array<long, 2> start) {
        std::set<ox::grid<char>::raw_iterator> processes;
        std::deque<ox::grid<char>::raw_iterator> queue{g.iterator_from_coord(start)};

        while (!queue.empty()) {
            auto top = queue.front();
            queue.pop_front();

            *top = '+';

            auto neighbours = g.cardinal_neighbour_range(top);
            auto next =
                    neighbours | ox::grid<char>::valid_index() | stdv::filter([](const auto& c) { return *c != '#'; });
            for (const auto& n : next) {
                if (processes.contains(n))
                    continue;
                queue.push_back(n);
                processes.insert(n);
            }
        }
    }

    // ==========================================
    // PART 2
    // ==========================================

    using index_list = std::vector<long>;
    std::array<long, 2> move_and_update(std::array<long, 2> pos, int amount, char dir, index_list& x, index_list& y) {
        switch (dir) {
            case 'U': y.push_back(pos[1] - amount); return {pos[0], pos[1] - amount};
            case 'L': x.push_back(pos[0] - amount); return {pos[0] - amount, pos[1]};
            case 'D': y.push_back(pos[1] + amount); return {pos[0], pos[1] + amount};
            case 'R': x.push_back(pos[0] + amount); return {pos[0] + amount, pos[1]};
            default: std::unreachable();
        }
    }

    enum DIR2 : u_int8_t {
        UP = 1 << 0,
        DOWN = 1 << 1,
        LEFT = 1 << 2,
        RIGHT = 1 << 3,
        CENTER = UP | DOWN | LEFT | RIGHT,
    };

    using index_list = std::vector<long>;
    long index_of(const index_list& l, long val) {
        return stdr::lower_bound(l, val) - l.begin();
    }

    std::array<long, 2> outer(long l, long r) {
        return {2 * l, 2 * r};
    }
    std::array<long, 2> outer_h(long l, long r) {
        return {2 * l + 1, 2 * r};
    }
    std::array<long, 2> outer_v(long l, long r) {
        return {2 * l, 2 * r + 1};
    }
    std::array<long, 2> inner(long l, long r) {
        return {2 * l + 1, 2 * r + 1};
    }

    void move_and_insert(std::array<long, 2>& pos, pipe_structure& grid3, int amount, char dir, const index_list& x,
                         const index_list& y) {
        auto old_y_pos = index_of(y, pos[1]);
        auto old_x_pos = index_of(x, pos[0]);
        assert(old_x_pos != long(x.size()));
        assert(old_y_pos != long(y.size()));

        switch (dir) {
            case 'U':
                {
                    pos[1] -= amount;
                    auto new_y_pos = index_of(y, pos[1]);
                    assert(new_y_pos != long(y.size()));
                    grid3[outer(old_x_pos, new_y_pos)] |= S | MainLoop;
                    grid3[outer_v(old_x_pos, new_y_pos)] |= V | MainLoop;
                    grid3[inner(old_x_pos, new_y_pos)] = Inside;
                    ++new_y_pos;
                    for (; new_y_pos < old_y_pos - 1; ++new_y_pos) {
                        grid3[outer(old_x_pos, new_y_pos)] |= V | MainLoop;
                        grid3[outer_v(old_x_pos, new_y_pos)] |= V | MainLoop;
                        grid3[inner(old_x_pos, new_y_pos)] = Inside;
                    }
                    if (new_y_pos != old_y_pos) {
                        grid3[outer(old_x_pos, new_y_pos)] |= V | MainLoop;
                        grid3[outer_v(old_x_pos, new_y_pos)] |= V | MainLoop;
                    }
                    grid3[outer(old_x_pos, old_y_pos)] |= N | MainLoop;
                    break;
                };
            case 'L':
                {
                    pos[0] -= amount;
                    auto new_x_pos = index_of(x, pos[0]);
                    assert(new_x_pos != long(x.size()));
                    grid3[outer(new_x_pos, old_y_pos)] |= E | MainLoop;
                    grid3[outer_h(new_x_pos, old_y_pos)] |= H | MainLoop;
                    grid3[inner(new_x_pos, old_y_pos - 1)] = Inside;
                    ++new_x_pos;
                    for (; new_x_pos < old_x_pos - 1; ++new_x_pos) {
                        grid3[outer(new_x_pos, old_y_pos)] |= H | MainLoop;
                        grid3[outer_h(new_x_pos, old_y_pos)] |= H | MainLoop;
                        grid3[inner(new_x_pos, old_y_pos - 1)] = Inside;
                    }
                    if (new_x_pos != old_x_pos) {
                        grid3[outer(new_x_pos, old_y_pos)] |= H | MainLoop;
                        grid3[outer_h(new_x_pos, old_y_pos)] |= H | MainLoop;
                    }
                    grid3[outer(old_x_pos, old_y_pos)] |= W | MainLoop;
                    break;
                };
            case 'D':
                {
                    pos[1] += amount;
                    auto new_y_pos = index_of(y, pos[1]);
                    assert(new_y_pos != long(y.size()));
                    grid3[outer(old_x_pos, old_y_pos)] |= S | MainLoop;
                    grid3[outer_v(old_x_pos, old_y_pos)] |= V | MainLoop;
                    grid3[inner(old_x_pos - 1, old_y_pos)] = Inside;
                    ++old_y_pos;
                    for (; old_y_pos < new_y_pos; ++old_y_pos) {
                        grid3[outer(old_x_pos, old_y_pos)] |= V | MainLoop;
                        grid3[outer_v(old_x_pos, old_y_pos)] |= V | MainLoop;
                        grid3[inner(old_x_pos - 1, old_y_pos)] = Inside;
                    }
                    grid3[outer(old_x_pos, new_y_pos)] |= N | MainLoop;
                    break;
                };
            case 'R':
                {
                    pos[0] += amount;
                    auto new_x_pos = index_of(x, pos[0]);
                    assert(new_x_pos != long(x.size()));

                    grid3[outer(old_x_pos, old_y_pos)] |= E | MainLoop;
                    grid3[outer_h(old_x_pos, old_y_pos)] |= H | MainLoop;
                    grid3[inner(old_x_pos, old_y_pos)] = Inside;
                    ++old_x_pos;
                    for (; old_x_pos < new_x_pos; ++old_x_pos) {
                        grid3[outer(old_x_pos, old_y_pos)] |= H | MainLoop;
                        grid3[outer_h(old_x_pos, old_y_pos)] |= H | MainLoop;
                        grid3[inner(old_x_pos, old_y_pos)] = Inside;
                    }
                    grid3[outer(new_x_pos, old_y_pos)] = W | MainLoop;
                    break;
                };
            default: std::unreachable();
        }
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input = get_from_input<dig_directions>(filename);
        ox::infinite_grid<char, 2> space;
        std::array pos{0l, 0l};
        space[pos] = '#';
        for (auto inst : input) {
            for (int i = 0; i < inst.amount; ++i) {
                pos = move(pos, inst.dir);
                space[pos] = '#';
            }
        }

        ox::grid<char> g{space.to_finite_grid('.')};
        auto start = find_first_inside(g);
        bfs_inside(g, start);

        auto res = stdr::count_if(g.get_raw(), [](char c) { return c != '.'; });
        myprintf("%zu\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto input = get_from_input<dig_directions>(filename);
        index_list x_bounds;
        index_list y_bounds;
        std::array pos{0l, 0l};
        for (auto inst : input) {
            pos = move_and_update(pos, int(inst.color), inst.dir2, x_bounds, y_bounds);
        }

        stdr::sort(x_bounds);
        stdr::sort(y_bounds);

        auto x_to_del = stdr::unique(x_bounds);
        auto y_to_del = stdr::unique(y_bounds);

        x_bounds.erase(x_to_del.begin(), x_to_del.end());
        y_bounds.erase(y_to_del.begin(), y_to_del.end());

        std::vector<long> x_width(x_bounds.size());
        std::vector<long> y_width(y_bounds.size());

        std::adjacent_difference(x_bounds.begin(), x_bounds.end(), x_width.begin());
        std::adjacent_difference(y_bounds.begin(), y_bounds.end(), y_width.begin());

        auto width = x_width.size();
        width += (width - 1);
        auto height = y_width.size();
        height += (height - 1);

        pipe_structure inside3(int(width), width * height);

        x_width.erase(x_width.begin());
        y_width.erase(y_width.begin());

        ox::grid<long> section_size(int(x_width.size()), x_width.size() * y_width.size());

        for (long y = 0; y < long(y_width.size()); ++y) {
            for (long x = 0; x < long(x_width.size()); ++x) {
                section_size[x, y] = x_width[x] * y_width[y];
            }
        }

        pos = {0l, 0l};
        for (auto inst : input) {
            move_and_insert(pos, inside3, int(inst.color), inst.dir2, x_bounds, y_bounds);
        }

        inside3.count_bounded();

        long res = 0;
        for (long y = 0; y < long(y_width.size()); ++y) {
            for (long x = 0; x < long(x_width.size()); ++x) {
                if (inside3[inner(x, y)] & Inside) {
                    res += section_size[x, y];
                }
            }
        }
        for (auto inst : input) {
            if (inst.dir2 == 'D' || inst.dir2 == 'L')
                res += inst.color;
        }

        myprintf("%ld\n", res + 1);
        return res + 1;
    }
} // namespace aoc2023::day18