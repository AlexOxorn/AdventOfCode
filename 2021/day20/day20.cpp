//
// Created by alexoxorn on 2021-12-23.
//

#include "day20.h"
#include <ox/grid.h>
#include <algorithm>
#include <numeric>

#define YEAR 2021
#define DAY 20

namespace day20 {
    constexpr std::array<std::pair<int, int>, 9> cardinal_directions = {{
           {-1, -1}, {0, -1}, {1, -1},
           {-1, 0}, {0, 0}, {1, 0},
           {-1, 1}, {0, 1}, {1, 1}
    }};

    std::vector<bool> parse_decoder(std::istream& in) {
        std::string s;
        std::vector<bool> to_return;
        to_return.reserve(513);
        while (std::getline(in, s) && !s.empty()) {
            std::transform(s.begin(), s.end(), std::back_inserter(to_return), [](char c) { return c == '#'; } );
        }
        return to_return;
    }

    std::pair<ox::grid<bool>, bool> next_image(const ox::grid<bool>& image, const std::vector<bool>& decoder, bool empty_space) {
        bool new_empty_space = empty_space ? decoder.back() : decoder.front();
        ox::grid<bool> new_image(image.get_width() + 2, (image.get_width() + 2) * (image.get_height() + 2));

        for(std::size_t j : stdv::iota(std::size_t(0), new_image.get_width())) {
            for (std::size_t i : stdv::iota(std::size_t(0), new_image.get_height())) {
                auto neighbours = cardinal_directions | stdv::transform([i, j, empty_space,&image](auto offset) {
                    auto get_opt = image.get_opt(i + offset.first - 1, j + offset.second - 1);
                    return get_opt.value_or(empty_space);
                });

                auto index = std::accumulate(neighbours.begin(), neighbours.end(), 0, [](int sum, bool next) {
                    return (sum << 1) + next;
                });

                new_image.get(i, j) = decoder[index];
            }
        }

        return {new_image, new_empty_space};
    }

    void print_image(const ox::grid<bool>& image) {
        image.leveled_foreach(
               [](bool i) { printf("%c", i ? '#' : '.'); },
               []() { printf("\n"); }
        );
    }

    static ox::grid<bool> source;
    static ox::grid<bool> image;
    static bool empty = false;
    static int puzzle_1_iteration_count = 2;
    static int puzzle_2_iteration_count = 50;
    static std::vector<bool> decoder;

    void puzzle1() {
        int iteration_count = 2;
        auto in_stream = GET_STREAM(input, ox::line);
        decoder = parse_decoder(in_stream);
        source = ox::grid<bool>(in_stream, [](char a) mutable { return a == '#'; });

        image = source;
        for (int i = 0; i < iteration_count; i++) {
            auto [new_image, new_empty] = next_image(image, decoder, empty);
            empty = new_empty;
            image = std::move(new_image);
        }

        auto count = std::count(image.get_raw().begin(), image.get_raw().end(), true);
        printf("Total number of # are %ld\n\n", count);
    }

    void puzzle2() {
        for (int i = puzzle_1_iteration_count; i < puzzle_2_iteration_count; i++) {
            auto [new_image, new_empty] = next_image(image, decoder, empty);
            empty = new_empty;
            image = std::move(new_image);
        }

        print_image(image);
        auto count = std::count(image.get_raw().begin(), image.get_raw().end(), true);
        printf("Total number of # are %ld\n", count);
    }
}
