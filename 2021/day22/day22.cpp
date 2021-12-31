//
// Created by alexoxorn on 2021-12-26.
//

#include "day22.h"
#include <functional>
#include <numeric>
#include <cmath>
#include <cassert>
#include <ox/ranges.h>

#define YEAR 2021
#define DAY 22

namespace day22 {
    struct cube {
        long x1, x2, y1, y2, z1, z2;
        auto operator<=>(const cube& other) const = default;

        [[nodiscard]] long area() const {
            return (x2 - x1 + 1l) * (y2 - y1 + 1l) * (z2 - z1 + 1l);
        }

        cube operator*(const cube& other) const {
            return {std::max(x1, other.x1),
                   std::min(x2, other.x2),
                   std::max(y1, other.y1),
                   std::min(y2, other.y2),
                   std::max(z1, other.z1),
                   std::min(z2, other.z2)};
        }

        [[nodiscard]] bool valid() const {
            return x2 >= x1 && y2 >= y1 && z2 >= z1;
        }
    };

    struct instruction : public cube {
        bool on;
    };

    std::vector<cube> distribute_intersection(
           const cube& base, std::random_access_iterator auto cube_begin, std::random_access_iterator auto cube_end) {
        auto x = stdr::subrange(cube_begin, cube_end)
               | stdv::transform([&](const cube& c) { return base * c; })
               | stdv::filter([](const cube& c) { return c.valid(); });
        return std::vector(x.begin(), x.end());
    }

    long union_area(std::random_access_iterator auto cube_begin, std::random_access_iterator auto cube_end) {
        if (cube_begin == cube_end)
            return 0l;
        auto head_end = std::prev(cube_end);
        auto& base = *head_end;
        auto distributed = distribute_intersection(base, cube_begin, head_end);
        long head_volume = union_area(cube_begin, std::prev(cube_end));
        long tail_volume = base.area();
        long tail_union = union_area(distributed.begin(), distributed.end());
        return head_volume + tail_volume - tail_union;
    }

    std::istream& operator>>(std::istream& in, instruction& inst) {
        std::string s;
        std::getline(in, s);
        char on_off[4] = {};

        sscanf(s.c_str(), "%s x=%ld..%ld,y=%ld..%ld,z=%ld..%ld", on_off,
               &inst.x1, &inst.x2, &inst.y1, &inst.y2, &inst.z1, &inst.z2);

        inst.on = (std::string("on") == on_off);
        return in;
    }

    static std::vector<instruction> instructions;

    template<long N>
    constexpr cube bounds{-1 * N, N, -1 * N, N, -1 * N, N};

    void solve(bool within_bounds = false) {
        long total_area = 0;
        for (auto& cube_itr : instructions
                      | ox::ranges::views::iterators
                      | stdv::filter([](const auto& inst) { return inst->on; })) {
            cube head = *cube_itr;
            if (within_bounds)
                head = head * bounds<50>;
            if (!head.valid())
                continue;
            long temp_volume = head.area();
            auto distributed = distribute_intersection(head, std::next(cube_itr), instructions.end());
            long negative_volume = union_area(distributed.begin(), distributed.end());
            long result_volume = temp_volume - negative_volume;
            assert(result_volume >= 0);
            total_area += result_volume;
        }
        printf("Total volume is %ld\n", total_area);
    }

    void puzzle1() {
        auto input = GET_STREAM(input, instruction);
        instructions = std::vector(input.begin(), input.end());
        solve(true);
    }

    void puzzle2() {
        solve();
    }
} // namespace day22