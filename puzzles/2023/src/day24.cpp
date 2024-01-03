#include "../../../common.h"
#include <format>
#include <ranges>
#include <cmath>
#include <Eigen/Dense>

namespace aoc2023::day24 {
    struct point {
        double x, y;
        bool operator==(const point&) const = default;

        [[nodiscard]] bool in_bound(double lower, double upper) const {
            return !(x < lower or y < lower or x > upper or y > upper);
        }
    };

    struct point3 {
        double x, y, z;
        bool operator==(const point3&) const = default;
    };

    struct hail_init {
        double x0, y0, z0;
        double vx, vy, vz;
        bool operator==(const hail_init&) const = default;
    };

    struct line_2d_equation {
        // y = ax + b;
        double a, b;
        line_2d_equation(hail_init h) {
            a = h.vy / h.vx;
            b = h.y0 - a * h.x0;
        }
    };

    STREAM_IN(hail_init, h) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "%lf, %lf, %lf @ %lf,  %lf, %lf", &h.x0, &h.y0, &h.z0, &h.vx, &h.vy, &h.vz);
        return in;
    }

    STREAM_OUT(hail_init, h) {
        return out << std::format("{}, {}, {} @ {}, {}, {}", h.x0, h.y0, h.z0, h.vx, h.vy, h.vz);
    }

    [[nodiscard]] point intersect(line_2d_equation l, line_2d_equation r) {
        double x_int = (r.b - l.b) / (l.a - r.a);
        double y_int = l.a * x_int + l.b;
        return {x_int, y_int};
    }

    std::optional<point> intersect(hail_init l, hail_init r) {
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
        if (tl < 0 or tr < 0) {
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

    bool parallel(hail_init l, hail_init r) {
        long lvx = std::abs(long(l.vx));
        long lvy = std::abs(long(l.vy));
        long lvz = std::abs(long(l.vz));
        long rvx = std::abs(long(r.vx));
        long rvy = std::abs(long(r.vy));
        long rvz = std::abs(long(r.vz));

        if (lvx % rvx != 0 or lvy % rvy != 0 or lvz % rvz != 0)
            return false;

        bool par = lvx / rvx == lvy / rvy and lvy / rvy == lvz / rvz;
        if (par) {
            std::cout << l << std::endl << r << std::endl << std::endl;
        }
        return par;
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

        long count = 0;
        for (auto i = 0zu; i < hail.size(); ++i) {
            for (auto j = i + 1; j < hail.size(); ++j) {
                auto res = intersect3d(hail[i], hail[j]);
                if (res) {
                    auto [x, y, z] = *res;
                    std::cout << hail[i] << std::endl;
                            std::cout << hail[j] << std::endl;
                    std::cout << std::format("{}, {}, {}\n\n", x, y, z);
                }
            }
        }
        printf("parallel count: %ld\n", count);
        return {};
    }
} // namespace aoc2023::day24