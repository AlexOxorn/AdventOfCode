//
// Created by alexoxorn on 2021-12-22.
//

#include "day19.h"
#include <ox/matrix.h>

#define YEAR 2021
#define DAY 18

namespace day19 {
    constexpr ox::matrix<int, std::array<int, 9>> rotate_z(3, 0, -1, 0, 1, 0, 0, 0, 0, 1);
    constexpr ox::matrix<int, std::array<int, 9>> rotate_y(3, 0, 0, -1, 0, 1, 0, 1, 0, 0);
    constexpr ox::matrix<int, std::array<int, 9>> rotate_x(3, 1, 0, 0, 0, 0, -1, 0, 1, 0 );
    constexpr ox::matrix<int, std::array<int, 9>> identitiy(3, 1, 0, 0, 0, 1, 0, 0, 0, 1 );

    constexpr std::array<ox::matrix<int, std::array<int, 9>>, 24> rotation_combinations() {
        std::array<ox::matrix<int, std::array<int, 9>>, 4 * 4 * 4> temp{};
        std::array<ox::matrix<int, std::array<int, 9>>, 24> to_return{};
        auto rotation_x = identitiy;
        auto rotation_y = identitiy;
        auto rotation_z = identitiy;
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

    int test() {
        auto x = rotation_combinations();
        for(auto v : x) {
            v.print_matrix();
            printf("----------------------\n");
        }
        return 0;
    }

    void puzzle1() {
        test();
    }

    void puzzle2() {}
}