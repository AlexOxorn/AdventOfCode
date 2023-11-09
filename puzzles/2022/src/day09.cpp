//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_set>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <thread>
#include <ox/canvas.h>
#include <ox/hash.h>

namespace aoc2022::day09 {
    using namespace std::chrono_literals;
    using coord = std::pair<int, int>;
    template <size_t N>
    using rope = std::array<coord, N>;
    using positionlog = std::unordered_set<coord, ox::pair_hash<>>;

    namespace drawer {
        static std::optional<ox::sdl_instance> drawing_window;
        static int current_min_x = 0;
        static int current_min_y = 0;
        static int current_max_x = 0;
        static int current_max_y = 0;

        [[maybe_unused]] constexpr int no_view = 0;
        [[maybe_unused]] constexpr int per_instruction_view = 1;
        [[maybe_unused]] constexpr int per_step_view = 2;

        constexpr bool display_part1 = true;
        constexpr bool display_part2 = true;
        constexpr int view_mode = no_view;
        constexpr auto frequency = 8ms;
        constexpr auto wait_after_complete = 5s;
        constexpr int scale = 5;

        void draw_point(int x, int y) {
            SDL_Rect r{(x - current_min_x) * scale, (current_max_y - y) * scale, scale, scale};
            SDL_RenderFillRect(drawing_window->screen_renderer(), &r);
        }

        template <size_t N>
        void draw_rope(const rope<N>& r, const positionlog& pos, bool with_head) {
            if (not drawing_window) {
                return;
            }

            auto [minI, maxI] = stdr::minmax(r | stdv::transform(&coord::first));
            auto [minJ, maxJ] = stdr::minmax(r | stdv::transform(&coord::second));

            current_min_y = std::min(drawer::current_min_y, minJ);
            current_min_x = std::min(drawer::current_min_x, minI);
            current_max_y = std::max(drawer::current_max_y, maxJ);
            current_max_x = std::max(drawer::current_max_x, maxI);

            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        drawing_window.reset();
                        continue;
                }
            }

            drawing_window->set_window_size(
                    std::make_pair(scale * (drawer::current_max_x - drawer::current_min_x + 1),
                                   scale * (drawer::current_max_y - drawer::current_min_y + 1)));

            drawing_window->clear_render();
            drawing_window->set_renderer_color(ox::named_colors::black);
            for (int j = current_max_y; j >= current_min_y; j--) {
                for (int i = current_min_x; i <= current_max_x; i++) {
                    if (pos.contains({i, j}))
                        draw_point(i, j);
                }
            }
            if (with_head) {
                drawing_window->set_renderer_color(ox::named_colors::red);
                for (int i = N - 1; i >= 0; i--) {
                    draw_point(r[i].first, r[i].second);
                }
            }
            drawing_window->redraw();
            std::this_thread::sleep_for(frequency);
        }

        void init_drawer() {
            if constexpr (view_mode >= per_instruction_view) {
                drawing_window.emplace("Bridge Viewer", true, std::make_pair(0, 0));
                drawing_window->background_color = ox::named_colors::white;
            }

        }
    } // namespace drawer

    struct instruction {
        int amount;
        char dir;
    };

    std::istream& operator>>(std::istream& in, instruction& inst) {
        in >> inst.dir;
        in >> inst.amount;
        return in;
    }

    int abs_distance(coord a, coord b) {
        return std::max(std::abs(a.first - b.first), std::abs(a.second - b.second));
    }

    int from_strong_ordering(std::strong_ordering i) {
        if (i == std::strong_ordering::less) {
            return -1;
        }
        if (i == std::strong_ordering::greater) {
            return 1;
        }
        return 0;
    }

    coord new_dir(coord head, coord tail) {
        if (abs_distance(head, tail) <= 1) {
            return {0, 0};
        }
        return {from_strong_ordering(head.first <=> tail.first), from_strong_ordering(head.second <=> tail.second)};
    }

    void move_rope(coord* head, coord* tail, coord dir, positionlog& pos, int remaining) {
        head->first += dir.first;
        head->second += dir.second;
        if (remaining == 0) {
            pos.insert(*head);
            return;
        }
        move_rope(tail, tail + 1, new_dir(*head, *tail), pos, remaining - 1);
    }

    template <size_t N>
    void move_rope(rope<N>& r, coord dir, positionlog& pos) {
        return move_rope(r.data(), r.data() + 1, dir, pos, N - 1);
    }

    template <size_t N, bool display>
    void apply_instruction(instruction inst, rope<N>& r, positionlog& pos) {
        for (int i = 0; i < inst.amount; i++) {
            switch (inst.dir) {
                case 'U': move_rope(r, {0, 1}, pos); break;
                case 'D': move_rope(r, {0, -1}, pos); break;
                case 'R': move_rope(r, {1, 0}, pos); break;
                case 'L': move_rope(r, {-1, 0}, pos); break;
            }
            if constexpr (display && drawer::view_mode == drawer::per_step_view)
                drawer::draw_rope(r, pos, true);
        }
    }

    template <size_t N, bool display>
    auto solve(puzzle_options filename) {
        positionlog visited{std::make_pair(0, 0)};
        rope<N> r{};
        auto instructions = get_stream<instruction>(filename);
        for (auto& inst : instructions) {
            apply_instruction<N, display> (inst, r, visited);
            if constexpr (display && drawer::view_mode)
                drawer::draw_rope(r, visited, true);
        }
        if constexpr (display && drawer::view_mode)
            drawer::draw_rope(r, visited, false);
        myprintf("The number of spaces the tail visited is %zu\n", visited.size());
        return visited.size();
    }

    answertype puzzle1(puzzle_options filename) {
        drawer::init_drawer();
        auto x = solve<2, drawer::display_part1>(filename);
        if (drawer::drawing_window && drawer::display_part1)
            std::this_thread::sleep_for(drawer::wait_after_complete);
        drawer::drawing_window.reset();
        return x;
    }

    answertype puzzle2(puzzle_options filename) {
        drawer::init_drawer();
        auto x = solve<10, drawer::display_part2>(filename);
        if (drawer::drawing_window && drawer::display_part2)
            std::this_thread::sleep_for(drawer::wait_after_complete);
        drawer::drawing_window.reset();
        return x;
    }
} // namespace aoc2022::day09