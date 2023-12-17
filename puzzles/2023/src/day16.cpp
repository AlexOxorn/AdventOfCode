#include "../../../common.h"
#include <ox/grid.h>

namespace aoc2023::day16 {
    enum DIR : uint8_t { UP = 1 << 0, DOWN = 1 << 1, LEFT = 1 << 2, RIGHT = 1 << 3 };
    struct light {
        DIR dir;
        long x;
        long y;

        [[nodiscard]] light move() const {
            auto self = *this;
            switch (self.dir) {
                case UP: --self.y; break;
                case DOWN: ++self.y; break;
                case LEFT: --self.x; break;
                case RIGHT: ++self.x; break;
            }
            return self;
        }
        [[nodiscard]] light reflect(char c) const {
            auto self = *this;
            if (c == '/')
                switch (self.dir) {
                    case UP: self.dir = RIGHT; break;
                    case LEFT: self.dir = DOWN; break;
                    case DOWN: self.dir = LEFT; break;
                    case RIGHT: self.dir = UP; break;
                }
            else if (c == '\\') {
                switch (self.dir) {
                    case UP: self.dir = LEFT; break;
                    case LEFT: self.dir = UP; break;
                    case DOWN: self.dir = RIGHT; break;
                    case RIGHT: self.dir = DOWN; break;
                }
            }
            return self;
        }
        [[nodiscard]] std::vector<light> split(char c) const {
            if (c == '|')
                switch (dir) {
                    case UP:
                    case DOWN:
                        return {
                                {dir, x, y}
                        };
                    case LEFT:
                    case RIGHT:
                        return {
                                {UP,   x, y},
                                {DOWN, x, y}
                        };
                }
            else if (c == '-') {
                switch (dir) {
                    case UP:
                    case DOWN:
                        return {
                                {LEFT,  x, y},
                                {RIGHT, x, y}
                        };
                    case LEFT:
                    case RIGHT:
                        return {
                                {dir, x, y}
                        };
                }
            }
            return {};
        }

        [[nodiscard]] std::vector<light> next(char c) const {
            if (c == '|' or c == '-')
                return this->split(c);
            if (c == '/' or c == '\\')
                return {this->reflect(c)};
            return {*this};
        }
    };

    using unit_type = std::pair<char, uint8_t>;
    struct mirror_space : ox::grid<unit_type> {
        using ox::grid<unit_type>::grid;

        void print() const {
            leveled_foreach(
                    [](const unit_type& t) {
                        auto& [c, e] = t;
                        printf("\033[%sm%c", e ? "31" : "0", c);
                    },
                    []() { printf("\n"); });
            printf("\033[0m\n");
        }

        void illuminate(light start = {RIGHT, 0, 0}) {
            auto& self = *this;
            std::vector<light> lights{start};
            while (!lights.empty()) {
                std::vector<light> next_set;
                for (const auto& current : lights) {
                    self[current.x, current.y].second |= current.dir;
                    auto moved = current.move();
                    if (!inbounds({moved.x, moved.y}))
                        continue;

                    auto [c, e] = self[moved.x, moved.y];

                    auto next = moved.next(c) | stdv::filter([e](const light& l) { return (e & l.dir) == 0; });
                    next_set.insert(next_set.begin(), next.begin(), next.end());
                }
                lights = std::move(next_set);
            }
        }

        void clear_energy() {
            stdr::for_each(data, [](auto& x) { x.second = 0; });
        }
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        mirror_space mirrors(input, [](char c) { return std::pair(c, false); });
        mirrors.illuminate();
        long illuminated = stdr::count_if(mirrors.data, std::identity(), &unit_type::second);
        printf("%ld\n", illuminated);
        return illuminated;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream(filename);
        mirror_space mirrors(input, [](char c) { return std::pair(c, false); });

        long max = 0;

#define COUNT_MAX \
    max = std::max(max, stdr::count_if(mirrors.data, std::identity(), &unit_type::second)); \
    mirrors.clear_energy();

        // TOP ROW
        for (long i = 0; i < mirrors.dimensions[0]; ++i) {
            mirrors.illuminate({DOWN, i, 0});
            COUNT_MAX
        }

        // BOTTOM ROW
        for (long i = 0; i < mirrors.dimensions[0]; ++i) {
            mirrors.illuminate({UP, i, mirrors.dimensions[1] - 1});
            COUNT_MAX
        }

        // LEFT EDGE
        for (long i = 0; i < mirrors.dimensions[1]; ++i) {
            mirrors.illuminate({RIGHT, 0, i});
            COUNT_MAX
        }

        // RIGHT EDGE
        for (long i = 0; i < mirrors.dimensions[1]; ++i) {
            mirrors.illuminate({LEFT, mirrors.dimensions[0] - 1, i});
            COUNT_MAX
        }

        printf("%ld\n", max);
        return max;
    }
} // namespace aoc2023::day16