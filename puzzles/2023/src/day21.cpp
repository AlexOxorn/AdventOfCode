#include "../../../common.h"
#include <ox/grid.h>
#include <set>
#include <unordered_set>
#include <string_view>
#include <format>
#include <cassert>

namespace aoc2023::day21 {
    constexpr static long INPUT_WIDTH = 131;
    constexpr static long MIDPOINT = INPUT_WIDTH / 2;
    constexpr static long STEPS = 26501365;
    constexpr static long LOOPS = (STEPS - (MIDPOINT)) / INPUT_WIDTH;
    constexpr static long LOOPS_WIDTH = 2 * LOOPS + 1;

    template <typename T>
    concept has_unique_object_representations = std::has_unique_object_representations_v<T>;

    static auto trivial_hash = []<has_unique_object_representations T>(const T& data) {
        std::string_view ss((char*) &data, sizeof(data));
        return std::hash<std::string_view>()(ss);
    };

    struct flower_map : ox::grid<char> {
        using ox::grid<char>::grid;
        using ox::grid<char>::const_raw_iterator;

        [[nodiscard]] const_raw_iterator find_start() const { return stdr::find(data, 'S'); }

        void print(const std::set<flower_map::const_raw_iterator>& highlight) const {
            leveled_iterators(
                    [&](auto c) {
                        if (highlight.contains(c)) {
                            myprintf("\033[41m ");
                        } else {
                            if (auto [x, y] = coord_from_index(c); x % 131 == 0 || y % 131 == 0)
                                myprintf("\033[44m%c\033[0m", *c);
                            else
                                myprintf("\033[0m%c", *c);
                        }
                    },
                    []() { myprintf("\033[0m\n"); });
        }

        void print(const std::set<flower_map::const_raw_iterator>& highlight,
                   const std::set<flower_map::const_raw_iterator>& seen) const {
            leveled_iterators(
                    [&](auto c) {
                        if (highlight.contains(c)) {
                            myprintf("\033[41m ");
                        } else if (seen.contains(c)) {
                            myprintf("\033[42m ");
                        } else {
                            myprintf("\033[0m%c", *c);
                        }
                    },
                    []() { myprintf("\033[0m\n"); });
        }

        void print(const std::unordered_set<std::array<long, 2>, decltype(trivial_hash)>& highlight,
                   const std::unordered_set<std::array<long, 2>, decltype(trivial_hash)>& seen) const {
            for (long j = 0; j < long(get_height()); ++j) {
                for (long i = 0; i < long(get_width()); ++i) {
                    if (highlight.contains({i, j})) {
                        myprintf("\033[41m ");
                    } else if (seen.contains({i, j})) {
                        myprintf("\033[42m ");
                    } else {
                        myprintf("\033[0m%c", (*this)[i, j]);
                    }
                }
                myprintf("\033[0m\n");
            }
        }
    };

    using positions = std::set<flower_map::const_raw_iterator>;
    using positions2 = std::unordered_set<std::array<long, 2>, decltype(trivial_hash)>;
    using area_map = std::unordered_map<char, long>;

    long calculate_area(const area_map& even, const area_map& odd) {
        using namespace std::string_literals;
        auto get_area_part_even = [even](char c) {
            return c ? even.at(c) : 0l;
        };
        auto get_area_even = [&get_area_part_even](const auto& s) {
            return stdr::fold_left(s | stdv::transform(get_area_part_even), 0l, std::plus());
        };


        long full_even = stdr::fold_left(even | stdv::values, 0l, std::plus());
        long full_odd = stdr::fold_left(odd | stdv::values, 0l, std::plus());
        long top = full_even - get_area_even("AB");
        long bottom = full_even - get_area_even("CD");
        long right = full_even - get_area_even("BD");
        long left = full_even - get_area_even("AC");

        long NE1 = full_even - even.at('B');
        long NE2 = odd.at('C');

        long NW1 = full_even - even.at('A');
        long NW2 = odd.at('D');

        long SE1 = full_even - even.at('D');
        long SE2 = odd.at('A');

        long SW1 = full_even - even.at('C');
        long SW2 = odd.at('B');

        long perimeter_area = 0;

        perimeter_area += top;
        perimeter_area += (LOOPS - 1) * (NE1 + NE2) + NE2;
        perimeter_area += right;
        perimeter_area += (LOOPS - 1) * (SE1 + SE2) + SE2;
        perimeter_area += bottom;
        perimeter_area += (LOOPS - 1) * (SW1 + SW2) + SW2;
        perimeter_area += left;
        perimeter_area += (LOOPS - 1) * (NW1 + NW2) + NW2;

        long inner_area = 0;
        for (long x = 1; x < LOOPS_WIDTH - 1; ++x) {
            long s_x = std::min(x, LOOPS_WIDTH - x - 1);
            for (long y = LOOPS - s_x + 1; y < LOOPS + s_x; ++y) {
                inner_area += (x + y) % 2 == 0 ? full_even : full_odd;
            }
        }

        return perimeter_area + inner_area;
    }

    /*
     A A A A O B B B B
     A A A I P J B B B
     A A I E P F J B B
     A I E E P F F J B
     K R R R T S S S L
     C M G G Q H H N D
     C C M G Q H N D D
     C C C M Q N D D D
     C C C C U D D D D
     */

    void area_test(const area_map& amap, const positions2& pos) {
#ifdef __cpp_lib_print
        std::print("{}\n", amap);
#endif
        assert(stdr::fold_left(amap | stdv::values, 0l, std::plus()) == long(pos.size()));
    }

#define DEFINE_CALCULATE_CORNER(X, BODY) \
    long calculate_##X(const positions2& seen, positions2& checked) { \
        long count = 0; \
        for (long i = 0; i < MIDPOINT; ++i) { \
            for (long j = 0; j < MIDPOINT - i; ++j) { \
                assert(!checked.contains BODY); \
                count += seen.contains BODY; \
                checked.insert BODY; \
            }; \
        } \
        return count; \
    }

#define DEFINE_CALCULATE_INNER(X, BODY) \
    long calculate_##X(const positions2& seen, positions2& checked) { \
        long count = 0; \
        for (long i = 2; i < MIDPOINT; ++i) { \
            for (long j = MIDPOINT + 1 - i; j < MIDPOINT; ++j) { \
                assert(!checked.contains BODY); \
                count += seen.contains BODY; \
                checked.insert BODY; \
            }; \
        } \
        return count; \
    }

#define DEFINE_CALCULATE_LINE(X, BODY) \
    long calculate_##X(const positions2& seen, positions2& checked) { \
        long count = 0; \
        for (long i = 1; i < MIDPOINT; ++i) { \
            [[maybe_unused]] long j = i; \
            assert(!checked.contains BODY); \
            count += seen.contains BODY; \
            checked.insert BODY; \
        } \
        return count; \
    }

#define DEFINE_CALCULATE_POINTS(X, POINT) \
    long calculate_##X(const positions2& seen, positions2& checked) { \
        assert(!checked.contains POINT); \
        checked.insert POINT; \
        return seen.contains POINT; \
    }

#define MIRROR(X) (INPUT_WIDTH - (X) -1)

    DEFINE_CALCULATE_CORNER(A, ({i, j}))
    DEFINE_CALCULATE_CORNER(B, ({MIRROR(i), j}))
    DEFINE_CALCULATE_CORNER(C, ({i, MIRROR(j)}))
    DEFINE_CALCULATE_CORNER(D, ({MIRROR(i), MIRROR(j)}))

    DEFINE_CALCULATE_INNER(E, ({i, j}))
    DEFINE_CALCULATE_INNER(F, ({MIRROR(i), j}))
    DEFINE_CALCULATE_INNER(G, ({i, MIRROR(j)}))
    DEFINE_CALCULATE_INNER(H, ({MIRROR(i), MIRROR(j)}))

    DEFINE_CALCULATE_LINE(I, ({MIRROR(i + MIDPOINT), j}))
    DEFINE_CALCULATE_LINE(J, ({i + MIDPOINT, j}))
    DEFINE_CALCULATE_LINE(M, ({i, j + MIDPOINT}))
    DEFINE_CALCULATE_LINE(N, ({MIRROR(i), j + MIDPOINT}))
    DEFINE_CALCULATE_LINE(P, ({INPUT_WIDTH / 2, j}))
    DEFINE_CALCULATE_LINE(Q, ({INPUT_WIDTH / 2, j + MIDPOINT}))
    DEFINE_CALCULATE_LINE(R, ({i, INPUT_WIDTH / 2}))
    DEFINE_CALCULATE_LINE(S, ({i + MIDPOINT, INPUT_WIDTH / 2}))

    DEFINE_CALCULATE_POINTS(K, ({0, MIDPOINT}))
    DEFINE_CALCULATE_POINTS(L, ({INPUT_WIDTH - 1, MIDPOINT}))
    DEFINE_CALCULATE_POINTS(O, ({MIDPOINT, 0}))
    DEFINE_CALCULATE_POINTS(U, ({MIDPOINT, INPUT_WIDTH - 1}))
    DEFINE_CALCULATE_POINTS(T, ({MIDPOINT, MIDPOINT}))

#define ADD_AREA(X) to_return[*#X] = calculate_##X(seen, checked)

    area_map get_area_part(const positions2& seen) {
        area_map to_return;
        positions2 checked;
        ADD_AREA(A);
        ADD_AREA(B);
        ADD_AREA(C);
        ADD_AREA(D);
        ADD_AREA(E);
        ADD_AREA(F);
        ADD_AREA(G);
        ADD_AREA(H);
        ADD_AREA(I);
        ADD_AREA(J);
        ADD_AREA(K);
        ADD_AREA(L);
        ADD_AREA(M);
        ADD_AREA(N);
        ADD_AREA(O);
        ADD_AREA(P);
        ADD_AREA(Q);
        ADD_AREA(R);
        ADD_AREA(S);
        ADD_AREA(T);
        ADD_AREA(U);
        assert(checked.size() == (INPUT_WIDTH * INPUT_WIDTH));
        return to_return;
    }

    positions calculate_steps(flower_map& map, flower_map::const_raw_iterator start, long num_steps) {
        positions curr{start};

        std::string line;

        for (long count = 0; count < num_steps; ++count) {
            positions next;
            for (auto pos : curr) {
                auto neighbours = map.cardinal_neighbour_range(pos);
                for (auto adj : neighbours | flower_map::const_valid_index()) {
                    if (*adj == '.' or *adj == 'S') {
                        next.insert(adj);
                    }
                }
            }
            std::swap(curr, next);
            next.clear();
        }

        return curr;
    }

    positions2 pos_to_pos2(const flower_map& map, const positions& data) {
        auto x = data | stdv::transform([&map](auto y) { return map.coord_from_index(y); });
        return positions2{x.begin(), x.end()};
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        flower_map map(input);
        positions res = calculate_steps(map, map.find_start(), 64);
        myprintf("%zu\n", res.size());
        return res.size();
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        flower_map map(input);

        positions area_template_iter_even = calculate_steps(map, map.find_start(), INPUT_WIDTH);
        positions area_template_iter_odd = calculate_steps(map, map.find_start(), INPUT_WIDTH + 1);
        positions2 area_template_even = pos_to_pos2(map, area_template_iter_even);
        positions2 area_template_odd = pos_to_pos2(map, area_template_iter_odd);
        area_map areas_even = get_area_part(area_template_even);
        area_map areas_odd = get_area_part(area_template_odd);
        area_test(areas_even, area_template_even);
        area_test(areas_odd, area_template_odd);
        long total_area = calculate_area(areas_even, areas_odd);
        myprintf("%ld\n", total_area);
        return total_area;
    }
} // namespace aoc2023::day21