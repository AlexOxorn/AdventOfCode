#include "../../../common.h"

#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <utility>
#include <random>
#include <chrono>
#include <ox/combinators.h>
#include <ox/parser.h>
#include <ox/canvas.h>
#include <ox/future/generator.h>

namespace aoc2023::day25 {
    using grid_type = std::multimap<std::string, std::string>;
    using edge = std::pair<std::string, std::string>;
    using path = std::vector<edge>;

    template <typename T, typename S>
    std::pair<S, T> reverse(std::pair<T, S> x) {
        return {x.second, x.first};
    }

    auto parse(ox::ifstream_container<ox::line>& in) {
        using namespace ox::parser::literals;
        using namespace ox::parser;

        std::set<std::string> names;
        grid_type grid;

        static std::string from;

        auto register_name = [&](std::string_view str) -> std::string {
            std::string name(str);
            if (!names.contains(name)) {
                names.insert(std::string(str));
            }
            return name;
        };

        auto header_callback = [&](void*, std::string_view str) {
            from = register_name(str.substr(0, str.size() - 1));
            return 0;
        };
        auto edge_callback = [&](void*, std::string_view str) {
            std::string to = register_name(str);
            grid.emplace(from, to);
            grid.emplace(to, from);
            return 0;
        };

        auto parser = String(header_callback) + " "_l + List(" ", String(edge_callback));

        for (const auto& line : in) {
            auto res = parser.parse(nullptr, line);
            if (!res) {
                std::cerr << "ERROR\n";
            }
        }
        return std::pair(names, grid);
    }

    long graph_size(const grid_type& grid, std::string from) {
        std::set<std::string> seen;
        std::deque<std::string> next{std::move(from)};
        while (!next.empty()) {
            auto curr = next.front();
            next.pop_front();
            if (seen.contains(curr))
                continue;
            seen.insert(curr);
            auto [adjStart, adjEnd] = grid.equal_range(curr);
            auto to_insert = stdr::subrange(adjStart, adjEnd) | stdv::values;
            next.insert(next.end(), to_insert.begin(), to_insert.end());
        }
        //        std::print("{}\n", seen);
        return seen.size();
    }

    constexpr double springCoef = 0.75;
    constexpr double edgeCoef = 2 * springCoef;
    constexpr double springRest = 200.0;
    constexpr double speedDamp = 0.9;
    constexpr double forceTimestep = 0.1;

    struct node {
        std::string name;
        double x = 0;
        double y = 0;
        bool locked = false;

        explicit node(std::string name) : name(name) {
            if (name == from or name == to) {
                locked = true;
            }
            if (name == to) {
                x = corner_x;
                y = corner_y;
            } else if (name == from) {
                x = 0.0;
                y = 0.0;
            }
            if (!locked) {
                static std::random_device rd;
                static std::mt19937 gen(rd());
                x = std::uniform_real_distribution<>(0.0, corner_x)(gen);
                y = std::uniform_real_distribution<>(0.0, corner_y)(gen);
            }
        }

        static std::string from;
        static std::string to;
        static double corner_x;
        static double corner_y;

        double temp_force_x = 0;
        double temp_force_y = 0;
        double temp_speed_x = 0;
        double temp_speed_y = 0;

        void reset_force() {
            temp_force_x = 0.0;
            temp_force_y = 0.0;
        }

        void update_speed() {
            temp_speed_x *= speedDamp;
            temp_speed_y *= speedDamp;
            temp_speed_x += forceTimestep * temp_force_x;
            temp_speed_y += forceTimestep * temp_force_y;
        }

        void update_position() {
            if (locked)
                return;
            x += temp_speed_x;
            y += temp_speed_y;
            y = std::clamp(y, 0.0, corner_y);
            x = std::clamp(x, 0.0, corner_x);
        }

        auto operator<=>(const node& other) const { return name <=> other.name; }
        bool operator==(const node& other) const { return name == other.name; }

        [[nodiscard]] double distance_from(const node& other) const {
            return std::sqrt((other.x - x) * (other.x - x) + (other.y - y) * (other.y - y));
        }
    };

    std::string node::from;
    std::string node::to;
    double node::corner_x = 0.0;
    double node::corner_y = 0.0;

    struct spring {
        node* left;
        node* right;

        [[nodiscard]] std::pair<double, double> get_center() const {
            double center_x = (left->x + right->x) / 2;
            double center_y = (left->y + right->y) / 2;
            return {center_x, center_y};
        }

        [[nodiscard]] double get_tension() const {
            if (left->locked or right->locked) {
                return 0.0;
            }
            return std::abs(left->x - right->x) + std::abs(left->y - right->y);
        }

        [[nodiscard]] edge get_edge() const { return {left->name, right->name}; }

        void apply_force() const {
            auto [center_x, center_y] = get_center();

            auto edgeStrength = left->locked or right->locked ? edgeCoef : springCoef;

            auto dist = left->distance_from(*right);
            auto x_ratio = dist != 0.0 ? std::abs(left->x - right->x) / dist : 1;
            auto y_ratio = dist != 0.0 ? std::abs(left->y - right->y) / dist : 1;

            auto left_x_force = -(left->x - center_x + (springRest * x_ratio / 2)) * edgeStrength;
            auto left_y_force = -(left->y - center_y + (springRest * y_ratio / 2)) * edgeStrength;

            left->temp_force_x += left_x_force;
            left->temp_force_y += left_y_force;
            right->temp_force_x -= left_x_force;
            right->temp_force_y -= left_y_force;
        }

        auto operator<=>(const spring&) const { return *left <=> *right; }
        auto operator==(const spring&) const { return *left == *right; }
    };

    void render(ox::sdl_instance& win, std::vector<node>& nodes, std::vector<spring>& springs,
                std::span<spring> most_tension) {
        win.background_color = ox::named_colors::white;
        win.clear_render();

        win.set_renderer_color(ox::named_colors::black);

        for (const node& n : nodes) {
            SDL_FRect rect{static_cast<float>(n.x - 2.0), static_cast<float>(n.y - 2.0), 5.0, 5.0};
            SDL_RenderDrawRectF(win.screen_renderer(), &rect);
        }
        for (const spring& s : springs) {
            auto text = win.get_texture(s.left->name + s.right->name);
            auto [cX, cY] = s.get_center();
            if (!std::isnan(cX) and !std::isinf(cX) and !std::isnan(cY) and !std::isinf(cY))
                text->render(cX, cY + 10);
            else
                text->render(s.left->x, s.left->y + 10);
            SDL_RenderDrawLineF(win.screen_renderer(), s.left->x, s.left->y, s.right->x, s.right->y);
        }

        win.set_renderer_color(ox::named_colors::red);
        for (const spring& s : most_tension) {
            auto text = win.get_texture(s.left->name + s.right->name);
            auto [cX, cY] = s.get_center();
            text->render(cX, cY + 10);
            SDL_RenderDrawLineF(win.screen_renderer(), s.left->x, s.left->y, s.right->x, s.right->y);
        }

        win.redraw();
    }

    std::array<edge, 3> simulate(const std::set<std::string>& names, grid_type grid, std::string from, std::string to) {
        node::from = from;
        node::to = to;

        ox::sdl_instance window("Pull", true);
        auto [w, h] = window.get_window_size();
        node::corner_x = w;
        node::corner_y = h;

        auto dots = names | stdv::transform([=](std::string s) { return node{s}; });
        std::vector nodes(dots.begin(), dots.end());

        auto get_spring = [&nodes, &window, from, to](edge s) {
            auto [f, t] = s;
            window.load_text(f + t, "/usr/share/fonts/truetype/ubuntu/Ubuntu-M.ttf", 10, std::format("{}-{}", f, t));
            return spring{stdr::find(nodes, f, &node::name).base(), stdr::find(nodes, t, &node::name).base()};
        };

        auto spring_stream = grid | stdv::filter(ox::apply_bind(std::less())) | stdv::transform(get_spring);
        std::vector springs(spring_stream.begin(), spring_stream.end());

        SDL_Event event;

        bool pause = true;
        auto tensions = springs;

        while (true) {
            using namespace std::literals::chrono_literals;
            auto next_frame = stdch::steady_clock::now() + 16ms;
            stdr::nth_element(tensions, tensions.begin() + 2, std::greater(), &spring::get_tension);
            render(window, nodes, springs, {tensions.data(), 3});
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        {
                            return {};
                        }
                    case SDL_KEYDOWN:
                        {
                            switch (event.key.keysym.sym) {
                                case SDLK_p: pause = !pause; break;
                                case SDLK_RETURN:
                                    return {tensions[0].get_edge(), tensions[1].get_edge(), tensions[2].get_edge()};
                            }
                            break;
                        }
                }
            }
            if (!pause) {
                stdr::for_each(nodes, &node::reset_force);
                stdr::for_each(springs, &spring::apply_force);
                stdr::for_each(nodes, &node::update_speed);
                stdr::for_each(nodes, &node::update_position);
            }
            std::this_thread::sleep_until(next_frame);
        }
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto in = get_stream(filename);
        auto [names, grid] = parse(in);

        std::string start;
        std::string end;
        if (grid.size() > 100) {
            start = "tqg";
            end = "bgm";
        } else {
            start = "rhn";
            end = "lhk";
        }

        std::array<edge, 3> real_cut =
                (do_print ? simulate(names, grid, start, end)
                          : std::array{edge("fql", "rmg"), edge("mfc", "vph"), edge("sfm", "vmt")});

        if (real_cut == std::array<edge, 3>{}) {
            return {};
        }

        for (auto c : real_cut) {
            grid.erase(stdr::find(grid, (grid_type::value_type) c));
            grid.erase(stdr::find(grid, (grid_type::value_type) reverse(c)));
        }

        auto left = graph_size(grid, start);
        auto right = graph_size(grid, end);
        myprintf("Graph Sizes: %ld * %ld = %ld\n", left, right, left * right);

        return left * right;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        return {};
    }
} // namespace aoc2023::day25