//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <queue>

namespace aoc2022::day18 {
    struct droplet {
        int x;
        int y;
        int z;
        using dir = std::pair<int droplet::*, int>;
        auto operator<=>(const droplet&) const = default;

        droplet move(dir d) {
            droplet to_return = *this;
            to_return.*d.first += d.second;
            return to_return;
        }

        friend std::istream& operator>>(std::istream& in, droplet& d) {
            char c;
            return in >> d.x >> c >> d.y >> c >> d.z;
        }
    };

    inline int dir_to_int(droplet::dir d) {
        if (d.first == &droplet::x) {
            return d.second;
        }
        if (d.first == &droplet::y) {
            return d.second * 2;
        }
        if (d.first == &droplet::z) {
            return d.second * 3;
        }
        return 0;
    }

    struct droplets_hash {
        size_t operator()(const droplet& d) const {
            return std::hash<int>()(d.x) + std::hash<int>()(d.y) + std::hash<int>()(d.z);
        }
    };

    using droplets_set = std::unordered_set<droplet, droplets_hash>;
    inline const auto droplets_contains_func =
            static_cast<bool (droplets_set::*)(const droplet&) const>(&droplets_set::contains);

    struct face {
        droplets_set* set;
        droplet drop;
        droplet::dir normal;

        bool operator==(const face&) const = default;

        face traverse_towards(droplet::dir d) {
            if (auto x = set->find(drop.move(normal).move(d)); x != set->end()) {
                return {
                        set, *x, {d.first, -d.second}
                };
            }
            if (auto x = set->find(drop.move(d)); x != set->end()) {
                return {set, *x, normal};
            }
            return {set, drop, d};
        }
    };

    struct face_hash {
        size_t operator()(const face& f) const { return droplets_hash()(f.drop) + dir_to_int(f.normal); }
    };

    using face_set = std::unordered_set<face, face_hash>;

    constexpr droplet::dir right{&droplet::x, 1};
    constexpr droplet::dir left{&droplet::x, -1};
    constexpr droplet::dir up{&droplet::y, 1};
    constexpr droplet::dir down{&droplet::y, -1};
    constexpr droplet::dir forward{&droplet::z, 1};
    constexpr droplet::dir back{&droplet::z, -1};
    constexpr std::array<droplet::dir, 6> adjacent{right, left, up, down, forward, back};

    answertype puzzle1(puzzle_options filename) {
        droplets_set processed_droplets;
        auto droplet_stream = get_stream<droplet>(filename);
        long surface_area = 0;

        for (droplet drop : droplet_stream) {
            auto adjacent_positions = adjacent | stdv::transform(std::bind_front(&droplet::move, drop));
            long shared_faces =
                    stdr::count_if(adjacent_positions, std::bind_front(droplets_contains_func, processed_droplets));
            surface_area += 6 - 2 * shared_faces;

            processed_droplets.emplace(drop);
        }
        myprintf("The total surface area of the droplets is %ld\n", surface_area);
        return surface_area;
    }

    answertype puzzle2(puzzle_options filename) {
        auto droplet_stream = get_stream<droplet>(filename);
        droplets_set droplets(droplet_stream.begin(), droplet_stream.end());
        face_set processed_faces;
        std::queue<face> next_in_line;

        droplet highest_point = stdr::max(droplets, std::less<>(), &droplet::y);
        next_in_line.push({&droplets, highest_point, up});

        while (!next_in_line.empty()) {
            face current = next_in_line.front();
            next_in_line.pop();
            if (processed_faces.contains(current))
                continue;
            processed_faces.insert(current);
            for (droplet::dir d : adjacent) {
                next_in_line.push(current.traverse_towards(d));
            }
        }

        myprintf("The total external surface area of the droplets is %zu\n", processed_faces.size());
        return processed_faces.size();
    }
} // namespace aoc2022::day18