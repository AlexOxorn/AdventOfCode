//
// Created by alexoxorn on 2021-12-22.
//

#include "day19.h"
#include <ox/matrix.h>
#include <cassert>
#include <queue>

#define YEAR 2021
#define DAY 19

namespace day19 {
    using transformation = ox::matrix<int>;
    using beacon = ox::matrix<int>;
    using scanner = std::vector<beacon>;

    static transformation rotate_z(3, 0, -1, 0, 1, 0, 0, 0, 0, 1);
    static transformation rotate_y(3, 0, 0, -1, 0, 1, 0, 1, 0, 0);
    static transformation rotate_x(3, 1, 0, 0, 0, 0, -1, 0, 1, 0);
    static transformation identity(3, 1, 0, 0, 0, 1, 0, 0, 0, 1);

    std::array<transformation, 24> rotation_combinations() {
        std::array<transformation, 4 * 4 * 4> temp{};
        std::array<transformation, 24> to_return{};
        auto rotation_x = identity;
        auto rotation_y = identity;
        auto rotation_z = identity;
        for (int i = 0; i < 4; i++, rotation_x = rotation_x * rotate_x) {
            for (int j = 0; j < 4; j++, rotation_y = rotation_y * rotate_y) {
                for (int k = 0; k < 4; k++, rotation_z = rotation_z * rotate_z) {
                    temp.at(i * 16 + j * 4 + k) = (rotation_x * rotation_y * rotation_z);
                }
            }
        }

        std::sort(temp.begin(), temp.end(), std::greater<>());
        auto x = std::unique(temp.begin(), temp.end());
        std::copy(temp.begin(), x, to_return.begin());
        return to_return;
    }

    static std::array<transformation, 24> all_3d_rotations = rotation_combinations();

    static std::vector<std::pair<scanner, ox::matrix<int>>> final_offsets;

    beacon parse_beacon(std::istream& in) {
        int x, y, z;
        char l, m;

        in >> x >> l >> y >> m >> z;
        assert(l == ',' && m == ',');

        return beacon(1, x, y, z);
    }

    scanner parse_scanner(std::istream& in) {
        scanner to_return;
        assert(in.peek() == '-');
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        while (in && in.peek() != '\n') {
            to_return.push_back(parse_beacon(in));
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return to_return;
    }

    std::vector<scanner> parse_input(std::istream& in) {
        std::vector<scanner> to_return;
        while (in) {
            to_return.push_back(parse_scanner(in));
        }
        return to_return;
    }

    std::optional<ox::matrix<int>> change_scanner_b_relative_to_a(const scanner& a, scanner& b) {
        for (const auto& beacon_from_one : a) {
            scanner a_cpy;
            a_cpy.reserve(a.size());
            std::transform(a.begin(), a.end(), std::back_inserter(a_cpy), [&beacon_from_one](auto x) { return x - beacon_from_one; });
            stdr::sort(a_cpy);

            for (auto rotation : all_3d_rotations) {
                scanner b_rotated;
                b_rotated.reserve(b.size());
                std::transform(b.begin(), b.end(), std::back_inserter(b_rotated), [&rotation](auto x) { return rotation * x; });

                for (const auto& beacon_from_b : b_rotated) {
                    scanner b_cpy;
                    b_cpy.reserve(b_rotated.size());
                    std::transform(b_rotated.begin(), b_rotated.end(), std::back_inserter(b_cpy), [&beacon_from_b](auto x) { return x - beacon_from_b; });
                    auto temp_first = b_cpy.front();
                    stdr::sort(b_cpy);

                    scanner merged;
                    merged.reserve(a_cpy.size() + b_cpy.size());
                    stdr::merge(a_cpy, b_cpy, std::back_inserter(merged));
                    auto unique_end = std::unique(merged.begin(), merged.end());
                    auto number_of_unique = std::distance(unique_end, merged.end());
                    if (number_of_unique >= 12) {
                        std::transform(b_cpy.begin(), b_cpy.end(), b.begin(), [&beacon_from_one](auto x) { return x + beacon_from_one; });
                        return beacon_from_one - beacon_from_b;
                    }
                }
            }
        }
        return std::nullopt;
    }

    void puzzle1() {
        auto stream(GET_STREAM(input, int));
        auto input = parse_input(stream);
        std::queue<std::pair<scanner, ox::matrix<int>>> todo;

        todo.push(std::make_pair(std::move(input.front()), ox::matrix<int>(1, 0, 0, 0)));
        input.erase(input.begin());

        while (!todo.empty()) {
            std::vector<scanner> new_input;
            auto s = todo.front();
            todo.pop();
            for (auto& it : input) {
                auto overlap = change_scanner_b_relative_to_a(s.first, it);
                if (overlap) {
                    todo.push(std::make_pair(std::move(it), *overlap));
                } else {
                    new_input.push_back(std::move(it));
                }
            }
            input = std::move(new_input);
            final_offsets.push_back(std::move(s));
        }

        for (auto& scanner : final_offsets) {
            stdr::sort(scanner.first);
        }

        auto full_list = std::accumulate(final_offsets.begin(), final_offsets.end(), scanner{}, [](const auto& sum, const auto& scanner) {
            std::vector<beacon> b;
            b.reserve(sum.size() + scanner.first.size());
            stdr::merge(sum, scanner.first, std::back_inserter(b));
            return b;
        });

        printf("Full list size: %zu\n", full_list.size());
        auto unique_end = std::unique(full_list.begin(), full_list.end());
        printf("unique list size: %zu\n", unique_end - full_list.begin());
    }

    void puzzle2() {
        int max = 0;

        for (auto x = final_offsets.begin(); x != final_offsets.end(); ++x) {
            for (auto y = x; y != final_offsets.end(); ++y) {
                ox::matrix<int> a = x->second;
                ox::matrix<int> b = y->second;
                max = std::max(max, std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]) + std::abs(a[2] - b[2]));
            }
        }

        printf("Largest distance is %d\n", max);
    }
} // namespace day19