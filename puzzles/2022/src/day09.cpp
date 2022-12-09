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

namespace aoc2022::day09 {
    namespace drawer {
        static std::optional<ox::sdl_instance> drawing_window;
        static int current_min_x = 0;
        static int current_min_y = 0;
        static int current_max_x = 0;
        static int current_max_y = 0;
        bool quit = false;
        constexpr int scale = 5;
    } // namespace drawer

    using namespace std::chrono_literals;
    using coord = std::pair<int, int>;
    template <size_t N>
    using long_rope = std::array<coord, N>;
    using rope = long_rope<2>;
    struct coord_hash {
        std::size_t operator()(const coord& c) const {
            return 2 * std::hash<int>()(c.first) + 3 * std::hash<int>()(c.second);
        }
    };
    using positionlog = std::unordered_set<coord, coord_hash>;

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
    void move_rope(long_rope<N>& r, coord dir, positionlog& pos) {
        return move_rope(r.data(), r.data() + 1, dir, pos, N - 1);
    }

    void draw_point(int x, int y) {
        using namespace drawer;
        SDL_Rect r{(x - current_min_x) * scale, (current_max_y - y) * scale, scale, scale};
        SDL_RenderFillRect(drawing_window->screen_renderer(), &r);
    }

    template <size_t N>
    void printSpace(const long_rope<N>& r, const positionlog& pos, bool with_head) {
        using namespace drawer;
        if (quit)
            return;

        auto [minposI, maxposI] = stdr::minmax(pos | stdv::transform(&coord::first));
        auto [minposJ, maxposJ] = stdr::minmax(pos | stdv::transform(&coord::second));
        auto [minropeI, maxropeI] = stdr::minmax(r | stdv::transform(&coord::first));
        auto [minropeJ, maxropeJ] = stdr::minmax(r | stdv::transform(&coord::second));

        current_min_y = std::min({drawer::current_min_y, minposJ, minropeJ});
        current_min_x = std::min({drawer::current_min_x, minposI, minropeI});
        current_max_y = std::max({drawer::current_max_y, maxposJ, maxropeJ});
        current_max_x = std::max({drawer::current_max_x, maxposI, maxropeI});

        if (not drawer::drawing_window) {
            drawing_window.emplace("Bridge Viewer", true);
            drawing_window->background_color = ox::named_colors::white;
        }
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_QUIT: quit = true; continue;
            }
        }

        drawing_window->set_window_size(std::make_pair(scale * (drawer::current_max_x - drawer::current_min_x + 1),
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
        std::this_thread::sleep_for(8ms);
    }

    template <size_t N>
    void apply_instruction(instruction inst, long_rope<N>& r, positionlog& pos) {
        for (int i = 0; i < inst.amount; i++) {
            switch (inst.dir) {
                case 'U': move_rope(r, {0, 1}, pos); break;
                case 'D': move_rope(r, {0, -1}, pos); break;
                case 'R': move_rope(r, {1, 0}, pos); break;
                case 'L': move_rope(r, {-1, 0}, pos); break;
            }
//            printSpace(r, pos, true);
        }
    }

    template <size_t N>
    void solve(const char* filename) {
        positionlog visited{std::make_pair(0, 0)};
        long_rope<N> r{};
        auto instructions = get_stream<instruction>(filename);
        for (auto& inst : instructions) {
            apply_instruction(inst, r, visited);
            printSpace(r, visited, true);
        }
        printf("The number of spaces the tail visited is %zu\n", visited.size());
    }

    void puzzle1(const char* filename) {
        solve<2>(filename);
        std::this_thread::sleep_for(5s);
        drawer::current_min_x = 0;
        drawer::current_min_y = 0;
        drawer::current_max_x = 0;
        drawer::current_max_y = 0;
    }

    void puzzle2(const char* filename) {
        solve<10>(filename);
        std::this_thread::sleep_for(5s);
    }
} // namespace aoc2022::day09