//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ox/grid.h>
#include <ox/graph.h>
#include <variant>
#include <vector>
#include <stack>
#include <numeric>
#include <thread>
#include <ox/canvas.h>
#include <ox/hash.h>

namespace aoc2022::day14 {
    using namespace std::chrono_literals;
    using coord = std::pair<int, int>;

    enum class occupied_by { rock, sand_still, sand_moving };
    enum class operation { cont, still, end };

    std::mutex drawing_mutex;

    constexpr coord start_position{500, 0};
    constexpr bool view = true;
    constexpr auto refresh_frequency = 16ms;
    constexpr auto update_frequency = 2ms;
    constexpr auto update_frequency2 = 100ns;
    constexpr auto after_puzzle_wait = 5s;
    constexpr int scale = 10;

    namespace drawer {
        static std::optional<ox::sdl_instance> drawing_window;

        void conditional_sleep(auto duration) {
            if (drawing_window)
                std::this_thread::sleep_for(duration);
        }
    } // namespace drawer

    struct grid : public std::unordered_map<coord, occupied_by, ox::pair_hash<>> {
        template <stdr::range R>
        requires std::is_base_of_v<std::string, typename R::value_type>
        explicit grid(R& r) : max_depth(0) {
            for (const auto& s : r) {
                add_line(s);
            }
        }

        void set_max() { max_depth = stdr::max(*this | stdv::keys | stdv::transform(&coord::second)) + 2; }

        auto locked_insert(const value_type& val) {
            std::lock_guard lock(drawing_mutex);
            return insert(val);
        }

        template <bool floor>
        operation move_sand(coord& sand) {
            if (sand.second + 1 >= max_depth) {
                return floor ? operation::still : operation::end;
            }

            if (!contains(std::make_pair(sand.first, sand.second + 1))) {
                sand.second++;
                return operation::cont;
            }

            if (!contains(std::make_pair(sand.first - 1, sand.second + 1))) {
                sand.first--;
                sand.second++;
                return operation::cont;
            }

            if (!contains(std::make_pair(sand.first + 1, sand.second + 1))) {
                sand.first++;
                sand.second++;
                return operation::cont;
            }

            return operation::still;
        }

        template <bool floor>
        operation drop_sand() {
            coord sand = start_position;
            operation res;
            std::vector<coord> path{sand};
            while ((res = move_sand<floor>(sand)) == operation::cont) {}
            if (res == operation::still) {
                locked_insert(std::make_pair(sand, occupied_by::sand_still));
                if (drawer::drawing_window)
                    drawer::conditional_sleep(floor ? update_frequency2 : update_frequency);
                if constexpr (floor)
                    return sand == start_position ? operation::end : operation::still;
                else
                    return operation::still;
            }
            return operation::end;
        };
    private:
        int max_depth;
        void add_line(const std::string& line) {
            long x1, x2, y1, y2;
            const char* str_ptr = line.data();

            x1 = strtol(str_ptr, const_cast<char**>(&str_ptr), 10);
            y1 = strtol(++str_ptr, const_cast<char**>(&str_ptr), 10);
            while (str_ptr < line.end().base()) {
                x2 = strtol(str_ptr += 4, const_cast<char**>(&str_ptr), 10);
                y2 = strtol(++str_ptr, const_cast<char**>(&str_ptr), 10);
                for (long i = x1; i <= x2; i++) {
                    for (long j = y1; j <= y2; j++) {
                        locked_insert(std::make_pair(std::make_pair(i, j), occupied_by::rock));
                    }
                }
                for (long i = x2; i <= x1; i++) {
                    for (long j = y2; j <= y1; j++) {
                        locked_insert(std::make_pair(std::make_pair(i, j), occupied_by::rock));
                    }
                }
                y1 = y2;
                x1 = x2;
            }
        }
    };

    namespace drawer {
        static int current_min_x = 324;
        static int current_min_y = -5;
        static int current_max_x = 676;
        static int current_max_y = 176;

        void draw_point(int x, int y) {
            SDL_Rect r{(x - current_min_x) * scale, (y - current_min_y) * scale, scale, scale};
            SDL_RenderFillRect(drawing_window->screen_renderer(), &r);
        }

        bool draw_cave(const grid& pos) {
            {
                std::lock_guard lock(drawing_mutex);
                if (not drawing_window) {
                    return false;
                }

                SDL_Event e;
                while (SDL_PollEvent(&e)) {
                    switch (e.type) {
                        case SDL_QUIT: drawing_window.reset(); continue;
                    }
                }

                auto [minI, maxI] = stdr::minmax(pos | stdv::keys | stdv::transform(&coord::first));
                auto [minJ, maxJ] = stdr::minmax(pos | stdv::keys | stdv::transform(&coord::second));

                current_min_y = std::min(current_min_y, minJ - 5);
                current_min_x = std::min(current_min_x, minI - 5);
                current_max_y = std::max(current_max_y, maxJ + 5);
                current_max_x = std::max(current_max_x, maxI + 5);

                drawing_window->set_window_size(std::make_pair(scale * (current_max_x - current_min_x + 1),
                                                               scale * (current_max_y - current_min_y + 1)));

                drawing_window->clear_render();
                for (const auto& [pos, type] : pos) {
                    auto& [x, y] = pos;
                    drawing_window->set_renderer_color(type == occupied_by::rock ? ox::named_colors::red1
                                                                                 : ox::named_colors::blue1);
                    draw_point(x, y);
                }

                drawing_window->set_renderer_color(ox::named_colors::green1);
                draw_point(start_position.first, start_position.second);

                drawing_window->redraw();
            }
            std::this_thread::sleep_for(refresh_frequency);
            return true;
        }

        void init_drawer() {
            if constexpr (view) {
                drawing_window.emplace("Cave Viewer", true, std::make_pair(0, 0));
                drawing_window->background_color = ox::named_colors::black;
            }
        }
    } // namespace drawer

    grid& get_grid(const char* filename) {
        static std::optional<grid> g;
        if (g)
            return *g;

        auto input = get_stream<ox::line>(filename);
        g.emplace(input);
        g->set_max();
        return *g;
    }

    template <bool floor>
    void solve(const char* filename) {
        grid& g = get_grid(filename);

        std::jthread a;
        if constexpr (view)
            a = std::jthread([&g](const std::stop_token& s) {
                while (!s.stop_requested() && drawer::drawing_window) {
                    drawer::draw_cave(g);
                }
            });
        while (g.drop_sand<floor>() != operation::end) {}
        auto sand = stdr::count(g | stdv::values, occupied_by::sand_still);
        printf("The amount of still sand before sand falls forever is %zu\n", sand);
        drawer::conditional_sleep(after_puzzle_wait);
    }

    void puzzle1(const char* filename) {
        drawer::init_drawer();
        solve<false>(filename);
    }

    void puzzle2(const char* filename) {
        solve<true>(filename);
    }
} // namespace aoc2022::day14