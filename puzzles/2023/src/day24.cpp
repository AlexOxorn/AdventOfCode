#include "../../../common.h"
#include <format>
#include <ranges>
#include <cmath>
#include <numbers>
#include <deque>
#include <numeric>
#include <random>
#include <set>
#include <csignal>
#include <unistd.h>
#include <ox/canvas.h>
#include <ox/utils.h>
#include <ox/matrix.h>

namespace aoc2023::day24 {
    struct point {
        double x, y;
        bool operator==(const point&) const = default;

        [[nodiscard]] bool in_bound(double lower, double upper) const {
            return !(x < lower or y < lower or x > upper or y > upper);
        }
    };

    struct point3 : std::array<double, 3> {
        bool operator==(const point3&) const = default;
    };

    template <typename T>
    struct hail_init_t {
        T x0, y0, z0;
        T vx, vy, vz;
        bool operator==(const hail_init_t&) const = default;

        std::array<T, 3> operator[](long t) const { return {x0 + t * vx, y0 + t * vy, z0 + t * vz}; }
    };

    using hail_init = hail_init_t<long>;

    struct line_2d_equation {
        // y = ax + b;
        double a, b;
        template <typename T>
        explicit line_2d_equation(hail_init_t<T> h) {
            a = h.vy / h.vx;
            b = h.y0 - a * h.x0;
        }
    };

    struct dots_2d_equation {
        long a, b;
        dots_2d_equation() = default;
        dots_2d_equation(hail_init h, long hail_init::* speed, long hail_init::* pos) {
            a = h.*speed;
            b = h.*pos;
        }
        dots_2d_equation(long slope, long init) : a{slope}, b{init} {}
        static std::optional<dots_2d_equation> from(std::pair<long, long> a, std::pair<long, long> b) {
            auto [ax, ay] = a;
            auto [bx, by] = b;
            if (ax == bx)
                return std::nullopt;
            if ((by - ay) % (bx - ax) != 0)
                return std::nullopt;
            auto slope = (by - ay) / (bx - ax);
            long init;
            bool overflow = __builtin_mul_overflow(slope, ax, &init);
            if (overflow)
                return std::nullopt;
            init = ay - init;
            return dots_2d_equation{slope, init};
        }
    };

    [[nodiscard]] point intersect(line_2d_equation l, line_2d_equation r) {
        double x_int = (r.b - l.b) / (l.a - r.a);
        double y_int = l.a * x_int + l.b;
        return {x_int, y_int};
    }

    [[nodiscard]] std::optional<long> intersect(dots_2d_equation l, dots_2d_equation r) {
        if (l.a == r.a)
            return std::nullopt;
        if ((r.b - l.b) % (l.a - r.a) != 0)
            return std::nullopt;
        long x_int = (r.b - l.b) / (l.a - r.a);
        return x_int;
    }

    template <typename T>
    std::optional<point> intersect(hail_init_t<T> l, hail_init_t<T> r, bool allow_neg_t = false) {
        auto [x, y] = intersect(line_2d_equation(l), line_2d_equation(r));
        /*
         * vx * t + x0 = x
         * vx * t = x - x0;
         * t = (x - x0) / vx;
         */
        if (std::isinf(x))
            return std::nullopt;
        double tl = (x - l.x0) / l.vx;
        double tr = (x - r.x0) / r.vx;
        if (!allow_neg_t and (tl < 0 or tr < 0)) {
            return std::nullopt;
        }
        return {
                {x, y}
        };
    };

    std::optional<point3> intersect3d(hail_init l, hail_init r) {
        auto in = intersect(l, r);
        return in.and_then([=](point p) -> std::optional<point3> {
            auto [x, y] = p;
            double tl = (x - l.x0) / l.vx;
            double tr = (x - r.x0) / r.vx;
            if (std::abs(tl - tr) < 0.001) {
                return std::nullopt;
            }
            if (std::abs((tl * l.vz + l.z0) - (tr * r.vz + r.z0)) > 0.001) {
                return std::nullopt;
            }
            return point3{x, y, (tl * l.vz + l.z0)};
        });
    }

    std::array<double, 3> project_onto(std::array<long, 3> src, std::array<long, 3> dest) {
        // *************
        // proj n -> m : m * ((n dot m) / ||m||^2)
        // *************

        std::array<double, 3> multip{};
        stdr::transform(src, dest, multip.begin(), std::multiplies());
        auto dot_product = std::accumulate(multip.begin(), multip.end(), 0.0);

        std::array<double, 3> dest_squared{};
        stdr::transform(dest, dest_squared.begin(), [](double d) { return d * d; });
        auto squared_mag = std::accumulate(dest_squared.begin(), dest_squared.end(), 0.0);

        std::array<double, 3> to_return{};
        stdr::transform(dest, to_return.begin(), [=](double d) { return d * (dot_product / squared_mag); });
        return to_return;
    }

    std::array<double, 3> project_to_plane(std::array<long, 3> src, std::array<long, 3> normal) {
        auto projection = project_onto(src, normal);

        std::array<double, 3> to_return{};
        stdr::transform(src, projection, to_return.begin(), std::minus());
        return to_return;
    }

    STREAM_IN(hail_init, h) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "%ld, %ld, %ld @ %ld, %ld, %ld", &h.x0, &h.y0, &h.z0, &h.vx, &h.vy, &h.vz);
        return in;
    }

    template <typename T>
    STREAM_OUT(hail_init_t<T>, h) {
        return out << std::format("{}, {}, {} @ {}, {}, {}", h.x0, h.y0, h.z0, h.vx, h.vy, h.vz);
    }

    STREAM_OUT(dots_2d_equation, d) {
        return out << std::format("{}t + {}", d.a, d.b);
    }

    template <typename T>
    std::array<double, 3> cross_prod(std::array<T, 3> a, std::array<long, 3> b) {
        return {(double) a[1] * b[2] - a[2] * b[1],
                (double) a[2] * b[0] - a[0] * b[2],
                (double) a[0] * b[1] - a[1] * b[0]};
    }

    template <typename T>
    double magnitude(std::array<T, 3> vec) {
        return std::sqrt((double) vec[0] * vec[0] + (double) vec[1] * vec[1] + (double) vec[2] * vec[2]);
    }

    std::array<double, 3> normalize(std::array<double, 3> vec) {
        auto scale = magnitude(vec);
        return {
                vec[0] / scale,
                vec[1] / scale,
                vec[2] / scale,
        };
    }

    std::array<ox::matrix<double>, 2> get_projection_matrices(std::array<long, 3> normal) {
        auto raw_axis = cross_prod(normal, {0, 0, 1l});
        auto axis = normalize(raw_axis);

        double theta = std::acos(normal[2] / magnitude(normal));

        auto cosTh = std::cos(theta);
        auto minCosTh = 1 - cosTh;
        auto sinTh = std::sin(theta);

        auto [ux, uy, uz] = axis;

        ox::matrix<double> rotate(3,
                                  {ux * ux * minCosTh + cosTh,
                                   ux * uy * minCosTh - uz * sinTh,
                                   ux * uz * minCosTh + uy * sinTh,
                                   ux * uy * minCosTh + uz * sinTh,
                                   uy * uy * minCosTh + cosTh,
                                   uy * uz * minCosTh - ux * sinTh,
                                   ux * uz * minCosTh - uy * sinTh,
                                   uy * uz * minCosTh + ux * sinTh,
                                   uz * uz * minCosTh + cosTh});
        ox::matrix<double> flatten(3, {1, 0, 0, 0, 1, 0});
        return {flatten, rotate};
    }

    hail_init_t<double> project_hail_to_plan(hail_init h, const ox::matrix<double>& projection) {
        auto vel = projection * ox::matrix<double>(1, {(double) h.vx, (double) h.vy, (double) h.vz});
        auto pos = projection * ox::matrix<double>(1, {(double) h.x0, (double) h.y0, (double) h.z0});
        if (projection.get_size() == 6)
            return {pos.at(0, 0), pos.at(0, 1), 0, vel.at(0, 0), vel.at(0, 1), 0};
        return {pos.at(0, 0), pos.at(0, 1), pos.at(0, 2), vel.at(0, 0), vel.at(0, 1), vel.at(0, 2)};
    }

    double find_average_variance(std::span<hail_init> hails, std::array<long, 3> normal) {
        auto [flatten, rotate] = get_projection_matrices(normal);
        auto project = flatten * rotate;
        std::vector<hail_init_t<double>> projections(hails.size());
        stdr::transform(hails, projections.begin(), [=](hail_init h) { return project_hail_to_plan(h, project); });

        std::vector<point> intersections;

        for (auto x : projections)
            for (auto y : projections) {
                if (x == y)
                    continue;
                auto inter = intersect(x, y, true);
                if (inter)
                    intersections.push_back(*inter);
            }

        point average{};
        for (auto inter : intersections) {
            average.x += inter.x;
            average.y += inter.y;
        }
        average.x /= (double) intersections.size();
        average.y /= (double) intersections.size();

        double variance = 0.0;

        for (auto inter : intersections) {
            variance += (average.x - inter.x) * (average.x - inter.x);
            variance += (average.y - inter.y) * (average.y - inter.y);
        }
        return variance / ((double) intersections.size() - 1);
    }

    std::array<long, 3> find_throwing_dir(std::span<hail_init> hails) {
        std::array<long, 3> normal = {1, 1, 1};
        auto current_variance = find_average_variance(hails, normal);

#define INC_AND_CHECK(X, OP) \
    temp_normal = normal; \
    temp_normal[X] OP 1; \
    if (stdr::any_of(temp_normal, [](long d) { return d == 0l; })) \
        temp_normal[X] OP 1; \
    new_variance = find_average_variance(hails, temp_normal); \
    printf("\t%7ld %7ld %7ld\n", temp_normal[0], temp_normal[1], temp_normal[2]); \
    printf("\t%f\n", new_variance); \
    if (new_variance < current_variance) { \
        max_normal = temp_normal; \
        current_variance = new_variance; \
    }
        while (true) {
            printf("%7ld %7ld %7ld\n", normal[0], normal[1], normal[2]);
            myprintf("%f\n", current_variance);
            std::array<long, 3> temp_normal{};
            std::array<long, 3> max_normal = normal;
            double new_variance;
            INC_AND_CHECK(0, +=)
            INC_AND_CHECK(0, -=)
            INC_AND_CHECK(1, +=)
            INC_AND_CHECK(1, -=)
            INC_AND_CHECK(2, +=)
            INC_AND_CHECK(2, -=)
            if (max_normal == normal) {
                myprintf("%f\n", current_variance);
                break;
            }
            normal = max_normal;
        }
        printf("%ld %ld %ld\n", normal[0], normal[1], normal[2]);
        return normal;
    }

    template <long MAX = 1000>
    std::array<long, 3> find_throwing_dir2(std::span<hail_init> hails) {
        std::array<long, 3> current_min{};
        double current_var = std::numeric_limits<double>::infinity();

        int precision = 100;
        std::vector<long> x(precision + 1);
        std::vector<long> y(precision + 1);
        std::vector<long> z(precision + 1);
        std::iota(x.begin(), x.end(), 153000 - precision / 2);
        std::iota(y.begin(), y.end(), 166800 - precision / 2);
        std::iota(z.begin(), z.end(), 141400 - precision / 2);
        //        std::random_device rd;
        //        std::mt19937 g(rd());
        //        stdr::shuffle(x, g);
        //        stdr::shuffle(y, g);
        //        stdr::shuffle(z, g);

        for (long i : x) {
            if (i == 0)
                continue;
            for (long j : y) {
                if (j == 0)
                    continue;
                for (long k : z) {
                    if (k == 0)
                        continue;

                    double var = find_average_variance(hails, {i, j, k});
                    if (var < current_var) {
                        current_var = var;
                        current_min = {i, j, k};

                        printf("[%5ld, %5ld, %5ld]\nVAR: %f\n", i, j, k, current_var);
                    };
                }
            }
        }

        printf("[%ld, %ld, %ld]\nVAR: %f\n", current_min[0], current_min[1], current_min[2], current_var);
        return current_min;
    }

    std::array<long, 3> neg(std::array<long, 3> l) {
        stdr::transform(l, l.begin(), std::negate<>());
        return l;
    }

    long t_of_min_distance(hail_init a, hail_init b) {
        long delta_v[3] = {a.vx - b.vx, a.vy - b.vy, a.vz - b.vz};
        long delta_p[3] = {a.x0 - b.x0, a.y0 - b.y0, a.z0 - b.z0};
        long dpdv[3];
        long dvdv[3];
        stdr::transform(delta_v, delta_p, dpdv, std::multiplies());
        stdr::transform(delta_v, delta_v, dvdv, std::multiplies());

        // SUM (dpdv)/dv^2
        double numerator = -std::accumulate(dpdv, dpdv + 3, 0.0);
        double denominator = std::accumulate(dvdv, dvdv + 3, 0.0);

        double result = numerator / denominator;

        long approx = (long) std::round(result);
        return std::max(0l, approx);
    }

    double distance_squared_at_t(hail_init a, hail_init b, long t) {
        auto pos1 = a[t];
        auto pos2 = b[t];
        std::array<double, 3> diff{};
        stdr::transform(pos1, pos2, std::begin(diff), std::minus());
        stdr::transform(diff, std::begin(diff), [](long l) { return (double) l * l; });
        return std::sqrt(std::accumulate(diff.begin(), diff.end(), 0.0));
    }

    struct min_dist {
        const hail_init* first;
        const hail_init* second;
        long time;
        double distance_squared;
    };

    min_dist min_dist_of_hail(const hail_init* index, std::span<hail_init> rest) {
        min_dist to_return{};
        to_return.distance_squared = std::numeric_limits<double>::infinity();
        to_return.first = index;
        const auto& ref = *index;

        for (const auto& part : rest) {
            if (part == ref)
                continue;
            long min_time = t_of_min_distance(ref, part);
            double dist = distance_squared_at_t(ref, part, min_time);
            if (dist < to_return.distance_squared) {
                to_return.second = &part;
                to_return.time = min_time;
                to_return.distance_squared = dist;
            }
        }

        return to_return;
    }

    template <typename Comp>
    min_dist min_dist_of_all_hail(std::span<hail_init> hail, Comp cmp) {
        std::vector<min_dist> all_min(hail.size());
        stdr::transform(hail, all_min.begin(), [&](hail_init& ref) { return min_dist_of_hail(&ref, hail); });
        return stdr::min(all_min, cmp, &min_dist::distance_squared);
    }

    std::array<dots_2d_equation, 3> find_slope2(std::array<long, 3> normal, const std::span<hail_init>& list) {
        auto& first = list.front();
        std::array<long, 2> sign{1, -1};

        for (long t = 1;; ++t) {
            for (long s : sign) {
                if (t % 1000000000 == 0)
                    printf("%15ld\n", t);
                std::pair<long, long> dot1;
                std::pair<long, long> dot2;
                std::optional<dots_2d_equation> eq[3];

                dot1 = std::pair(t, first.vx * t + first.x0);
                dot2 = std::pair(t + s, dot1.second + normal[0]);
                eq[0] = dots_2d_equation::from(dot1, dot2);

                dot1 = std::pair(t, first.vy * t + first.vy);
                dot2 = std::pair(t + s, dot1.second + normal[1]);
                eq[0] = dots_2d_equation::from(dot1, dot2);

                dot1 = std::pair(t, first.vz * t + first.z0);
                dot2 = std::pair(t + s, dot1.second + normal[2]);
                eq[0] = dots_2d_equation::from(dot1, dot2);

                bool pass;

                if (!stdr::all_of(eq, [](auto x) { return x.has_value(); })) {
                    continue;
                }

                pass = stdr::all_of(list.begin() + 2, list.end(), [&](hail_init l) {
                    dots_2d_equation rep(l, &hail_init::vx, &hail_init::x0);
                    auto time = intersect(*eq[0], rep).value_or(-1);

                    rep = {l, &hail_init::vy, &hail_init::y0};
                    auto time2 = intersect(*eq[1], rep).value_or(-1);
                    if (time2 != time)
                        return false;

                    rep = {l, &hail_init::vz, &hail_init::z0};
                    time2 = intersect(*eq[2], rep).value_or(-1);
                    if (time2 != time)
                        return false;

                    return true;
                });

                if (pass) {
                    std::array<dots_2d_equation, 3> to_return{};
                    stdr::transform(eq, to_return.begin(), [](auto x) { return *x; });
                    return to_return;
                }
            }
        }
    }

    std::pair<point, point> line_box(hail_init_t<double> hailstone, double h, double w) {
        auto left = intersect(hailstone, {0, 0, 0, 0.00001, 1, 0});
        auto right = intersect(hailstone, {w, 0, 0, 0.00001, 1, 0});
        auto top = intersect(hailstone, {0, 0, 0, 1, 0, 0});
        auto bottom = intersect(hailstone, {0, h, 0, 1, 0, 0});

        std::vector<point> p;

        if (left and left->y > h)
            left.reset();
        if (right and right->y > h)
            right.reset();
        if (top and top->x > w)
            top.reset();
        if (bottom and bottom->x > w)
            bottom.reset();

        if (left)
            p.push_back(*left);
        if (right)
            p.push_back(*right);
        if (top)
            p.push_back(*top);
        if (bottom)
            p.push_back(*bottom);

        if (p.empty())
            return {};

        if (p.size() == 1)
            p.push_back({hailstone.x0, hailstone.y0});

        return {p[0], p[1]};
    }

    void render(std::span<hail_init> hail, std::array<long, 3> normal, ox::sdl_instance& window, bool show_box,
                bool zoom) {
        window.background_color = ox::named_colors::white;
        window.clear_render();

        window.set_renderer_color(ox::named_colors::black);

        auto [w, h] = window.get_window_size();
        auto [flat, rot] = get_projection_matrices(normal);
        //        rot.print_matrix();
        auto flattended_v = hail | stdv::transform([&](hail_init h) { return project_hail_to_plan(h, flat * rot); });
        std::vector<hail_init_t<double>> projections(flattended_v.begin(), flattended_v.end());

        std::vector<point> intersections;
        for (auto x : projections)
            for (auto y : projections) {
                if (x == y)
                    continue;
                auto inter = intersect(x, y, true);
                if (inter)
                    intersections.push_back(*inter);
            }

        if (intersections.empty()) {
            window.redraw();
            return;
        }

        // ==================================
        // Remove Outliers
        // ==================================
        point average{};
        static double variance_val = std::nan("");

        for (long i = 0l; i < 2; ++i) {
            // Initial Average
            average = {};
            for (auto inter : intersections) {
                average.x += inter.x;
                average.y += inter.y;
            }
            average.x /= (double) intersections.size();
            average.y /= (double) intersections.size();

            // Initial Variance
            std::vector<std::pair<point, double>> variances;
            variances.reserve(intersections.size());

            for (auto inter : intersections) {
                variances.emplace_back(inter,
                                       (average.x - inter.x) * (average.x - inter.x)
                                               + (average.y - inter.y) * (average.y - inter.y));
            }

            if (i == 0l) {
                stdr::sort(variances, std::greater(), &std::pair<point, double>::second);
                for (auto [inter, var] : variances | stdv::take(variances.size() / 2)) {
                    erase(intersections, inter);
                }
            } else {
                auto variance_val_view = variances | stdv::values
                                       | stdv::transform([&](double d) { return d / (variances.size() - 1); });
                auto total_var = std::accumulate(variance_val_view.begin(), variance_val_view.end(), 0.0);
                if (total_var < variance_val)
                    myprintf("\033[31mVariance: %f\033[0m\n", variance_val = total_var);
                else
                    myprintf("Variance: %f\n", variance_val = total_var);
            }
        }
        auto [minX, maxX] = stdr::minmax(projections, std::less(), &hail_init_t<double>::x0);
        auto [minY, maxY] = stdr::minmax(projections, std::less(), &hail_init_t<double>::y0);
        auto Xdist = maxX.x0 - minX.x0;
        auto Ydist = maxY.y0 - minX.y0;

        auto leftBound = minX.x0 - std::max(1000.0, Xdist / 2);
        auto topBound = minY.y0 - std::max(1000.0, Ydist / 2);

        auto Xscale = w / (4 * std::max(1000.0, Xdist / 2));
        auto Yscale = h / (8 * std::max(1000.0, Ydist / 2));

        if (zoom) {
            Xdist = w / 1000.0;
            Ydist = h / 1000.0;
            leftBound = average.x - Xdist / 2;
            topBound = average.y - Ydist / 2;
            Xscale = 1000;
            Yscale = 1000;
        }

        for (auto hailstone : projections) {
            hailstone.x0 -= leftBound;
            hailstone.y0 -= topBound;
            hailstone.x0 *= Xscale;
            hailstone.vx *= Xscale;
            hailstone.y0 *= Yscale;
            hailstone.vy *= Yscale;

            auto [start, end] = line_box(hailstone, h, w);

            SDL_RenderDrawLineF(window.screen_renderer(), start.x, start.y, end.x, end.y);
        }

        window.set_renderer_color(ox::named_colors::blue, 0x20);
        if (show_box)
            for (auto inter : intersections) {
                inter.x -= leftBound;
                inter.y -= topBound;
                inter.x *= Xscale;
                inter.y *= Yscale;

                SDL_FRect rect(inter.x - 10, inter.y - 10, 21, 21);
                SDL_RenderDrawRectF(window.screen_renderer(), &rect);
            }

        //        window.set_renderer_color(ox::named_colors::green, 0x20);
        //        for (auto inter : projections) {
        //            inter.x0 -= leftBound;
        //            inter.y0 -= topBound;
        //            inter.x0 *= Xscale;
        //            inter.y0 *= Yscale;
        //
        //            SDL_FRect rect(inter.x0 - 5, inter.y0 - 5, 11, 11);
        //            SDL_RenderDrawRectF(window.screen_renderer(), &rect);
        //        }

        window.set_renderer_color(ox::named_colors::red, 0x20);

        average.x -= leftBound;
        average.y -= topBound;
        average.x *= Xscale;
        average.y *= Yscale;

        SDL_FRect rect(average.x - 2, average.y - 2, 5, 5);
        SDL_RenderDrawRectF(window.screen_renderer(), &rect);

        window.redraw();
    }

    std::array<long, 3> simulate(std::span<hail_init> hail, bool skip = false) {
        std::array<long, 3> normal = {154019219901970386, 168458521767780198, 143590835221107905};
        if (skip)
            return normal;

        ox::sdl_instance window("Cross Section", true);
        auto size = std::min(10zu, hail.size());
        SDL_Event event;
        auto begin = hail.begin();
        bool show_box = true;
        bool zoom = false;
        myprintf("%7ld, %7ld, %7ld | %zu\n", normal[0], normal[1], normal[2], size);
        render({hail.begin(), size}, normal, window, show_box, zoom);
        auto reduce_normal = [&]() {
            auto gcd = std::accumulate(normal.begin(), normal.end(), normal.front(), std::gcd<long, long>);
            stdr::transform(normal, normal.begin(), [=](long l) { return l / gcd; });
        };
        while (true) {
            SDL_PollEvent(&event);
            switch (event.type) {
                case SDL_QUIT:
                    {
                        return {};
                    }
                case SDL_KEYDOWN:
                    {
                        switch (event.key.keysym.sym) {
                            case SDLK_q: normal[0]++; break;
                            case SDLK_a: normal[0]--; break;
                            case SDLK_w: normal[1]++; break;
                            case SDLK_s: normal[1]--; break;
                            case SDLK_e: normal[2]++; break;
                            case SDLK_d: normal[2]--; break;
                            case SDLK_r:
                                normal[0] *= 10;
                                normal[1] *= 10;
                                normal[2] *= 10;
                                break;
                            case SDLK_f:
                                normal[0] /= 10;
                                normal[1] /= 10;
                                normal[2] /= 10;
                                break;
                            case SDLK_t: begin++; break;
                            case SDLK_g: begin--; break;
                            case SDLK_u: size = std::min(size + 1, hail.size()); break;
                            case SDLK_j: size = std::min(size - 1, hail.size()); break;
                            case SDLK_i:
                                size = std::min(10zu, hail.size());
                                begin = hail.begin();
                                break;
                            case SDLK_k:
                                size = hail.size();
                                begin = hail.begin();
                                break;
                            case SDLK_o: show_box = !show_box; break;
                            case SDLK_p: zoom = !zoom; break;
                            case SDLK_l: reduce_normal(); break;
                            case SDLK_RETURN:
                                {
                                    reduce_normal();
                                    return normal;
                                }
                        }
                        myprintf("%7ld, %7ld, %7ld | %zu\n", normal[0], normal[1], normal[2], size);
                        render({begin, size}, normal, window, show_box, zoom);
                        break;
                    }
            }
        }
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto hail = get_from_input<hail_init>(filename);
        bool example = hail.size() < 10zu;

        double lower_bound = example ? 7.0 : 200000000000000.0;
        double higher_bound = example ? 27.0 : 400000000000000.0;

        long count = 0;
        for (auto i = 0zu; i < hail.size(); ++i) {
            for (auto j = i + 1; j < hail.size(); ++j) {
                auto res = intersect(hail[i], hail[j]);
                count += res.transform([=](point p) { return p.in_bound(lower_bound, higher_bound); }).value_or(0);
            }
        }

        printf("%ld\n", count);

        return count;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto hail = get_from_input<hail_init>(filename);

        long minX = std::numeric_limits<long>::max();
        long minY = std::numeric_limits<long>::max();
        long minZ = std::numeric_limits<long>::max();
        long maxX = std::numeric_limits<long>::min();
        long maxY = std::numeric_limits<long>::min();
        long maxZ = std::numeric_limits<long>::min();

        auto clamp =
                [](long& minref, long& maxref, const hail_init& stone, long hail_init::* speed, long hail_init::* pos) {
                    if (stone.*speed < 0)
                        minref = std::min(minref, stone.*pos);
                    else
                        maxref = std::max(maxref, stone.*pos);
                };

        for (auto stone : hail) {
            clamp(minX, maxX, stone, &hail_init::vx, &hail_init::x0);
            clamp(minY, maxY, stone, &hail_init::vy, &hail_init::y0);
            clamp(minZ, maxZ, stone, &hail_init::vz, &hail_init::z0);
        }

        std::cout << std::format("X: {:15} to {:15} : {:15}\n", minX, maxX, maxX - minX);
        std::cout << std::format("Y: {:15} to {:15} : {:15}\n", minY, maxY, maxY - minY);
        std::cout << std::format("Z: {:15} to {:15} : {:15}\n", minZ, maxZ, maxZ - minZ);


        std::cout << std::format("Prism(({}, {}, {}), ({}, {}, {}), ({}, {}, {}), ({}, {}, {}), ({}, {}, {}))\n",
                                 minX,
                                 minY,
                                 minZ,
                                 maxX,
                                 minY,
                                 minZ,
                                 maxX,
                                 maxY,
                                 minZ,
                                 minX,
                                 maxY,
                                 minZ,
                                 minX,
                                 minY,
                                 maxZ
                                 );

        return {};
        std::array<long, 3> normal = simulate(hail, false);
        if (normal == std::array{0l, 0l, 0l}) {
            return {};
        }

        auto [x, y, z] = find_slope2(normal, {hail.data(), std::min(10zu, hail.size())});

        std::cout << x << std::endl << y << std::endl << z << std::endl;
        return {};
    }
} // namespace aoc2023::day24