#include "../../../common.h"
#include <format>
#include <ranges>
#include <cmath>
#include <deque>
#include <sstream>
#include <Eigen/Dense>
#include <ox/utils.h>
#include <ox/math.h>

namespace aoc2023::day24 {
    struct point {
        double x, y;
        bool operator==(const point&) const = default;

        [[nodiscard]] bool in_bound(double lower, double upper) const {
            return !(x < lower or y < lower or x > upper or y > upper);
        }
    };

    struct hail_init {
        long x0, y0, z0;
        long vx, vy, vz;

        void geogebra_ray_2d(std::ostream& o = std::cout, long hail_init::* i = &hail_init::x0,
                             long hail_init::* j = &hail_init::y0, long hail_init::* vi = &hail_init::vx,
                             long hail_init::* vj = &hail_init::vy) {
            o << std::format("Ray(({}, {}), Vector(({}, {}))\n", this->*i, this->*j, this->*vi, this->*vj);
            o << std::format("({}, {}) + a * Vector(({}, {}))\n", this->*i, this->*j, this->*vi, this->*vj);
        }

        void geogebra_ray_3d(std::ostream& o = std::cout) {
            o << std::format("Ray(({}, {}, {}), Vector(({}, {}, {}))\n", x0, y0, z0, vx, vy, vz);
            o << std::format("({}, {}, {}) + a * Vector(({}, {}, {}))\n", x0, y0, z0, vx, vy, vz);
        }
    };

    struct line_2d_equation {
        // y = ax + b;
        double a, b;

        line_2d_equation(double a, double b) : a{a}, b{b} {}

        explicit line_2d_equation(hail_init h) {
            a = (double) h.vy / h.vx;
            b = h.y0 - a * h.x0;
        }

        static std::optional<line_2d_equation> from(std::pair<long, long> a, std::pair<long, long> b) {
            auto [ax, ay] = a;
            auto [bx, by] = b;
            if (ax == bx)
                return std::nullopt;
            if ((by - ay) % (bx - ax) != 0)
                return std::nullopt;
            double slope = double(by - ay) / (bx - ax);
            double init = ay - (ax * slope);
            if (std::isinf(init))
                return std::nullopt;
            return line_2d_equation{slope, init};
        }
    };

    [[nodiscard]] point intersect(line_2d_equation l, line_2d_equation r) {
        double x_int = (r.b - l.b) / (l.a - r.a);
        double y_int = l.a * x_int + l.b;
        return {x_int, y_int};
    }

    std::optional<point> intersect(hail_init l, hail_init r, bool allow_neg_t = false) {
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

    STREAM_IN(hail_init, h) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "%ld, %ld, %ld @ %ld, %ld, %ld", &h.x0, &h.y0, &h.z0, &h.vx, &h.vy, &h.vz);
        return in;
    }

    STREAM_OUT(hail_init, h) {
        return out << std::format("{}, {}, {} @ {}, {}, {}", h.x0, h.y0, h.z0, h.vx, h.vy, h.vz);
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

        myprintf("%ld\n", count);
        return count;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
#define DEF_VARS(X) auto [hx##X, hy##X, hz##X, vhx##X, vhy##X, vhz##X] = hail[X - 1]
#define SANDY_ROW(X, Y) \
    {(double) vh##Y##1 - vh##Y##X, (double) vhx##X - vhx1, (double) h##Y##X - h##Y##1, (double) hx1 - hx##X}
#define SANDY_ANSWER(X, Y) {(double) hx1 * vh##Y##1 - h##Y##1 * vhx1 - hx##X * vh##Y##X + h##Y##X * vhx##X}

        auto hail = get_from_input<hail_init>(filename);
        DEF_VARS(1);
        DEF_VARS(2);
        DEF_VARS(3);
        DEF_VARS(4);
        DEF_VARS(5);

        Eigen::MatrixXd AY{SANDY_ROW(2, y), SANDY_ROW(3, y), SANDY_ROW(4, y), SANDY_ROW(5, y)};
        Eigen::MatrixXd AZ{SANDY_ROW(2, z), SANDY_ROW(3, z), SANDY_ROW(4, z), SANDY_ROW(5, z)};
        Eigen::VectorXd yY{SANDY_ANSWER(2, y), SANDY_ANSWER(3, y), SANDY_ANSWER(4, y), SANDY_ANSWER(5, y)};
        Eigen::VectorXd yZ{SANDY_ANSWER(2, z), SANDY_ANSWER(3, z), SANDY_ANSWER(4, z), SANDY_ANSWER(5, z)};

        Eigen::VectorXd xY = AY.lu().solve(yY);
        Eigen::VectorXd xZ = AZ.lu().solve(yZ);

        myprintf("rx0: %f\n", xY[0]);
        myprintf("ry0: %f\n", xY[1]);
        myprintf("rz0: %f\n", xZ[1]);
        myprintf("vrx: %f\n", xY[2]);
        myprintf("vry: %f\n", xY[3]);
        myprintf("vrz: %f\n", xZ[3]);
        return long(std::round(xY[0] + xY[1] + xZ[1]));
    }
} // namespace aoc2023::day24