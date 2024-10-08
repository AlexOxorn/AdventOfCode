#include "../../../common.h"
#include <format>
#include <ranges>
#include <cmath>
#include <deque>
#include <numeric>
#include <random>
#include <sstream>
#include <ox/canvas.h>
#include <ox/utils.h>
#include <ox/matrix.h>
#include <ox/math.h>
#include <ox/future/generator.h>

namespace aoc2023::day24 {
    struct point {
        double x, y;
        bool operator==(const point&) const = default;

        [[nodiscard]] bool in_bound(double lower, double upper) const {
            return !(x < lower or y < lower or x > upper or y > upper);
        }
    };

    using point3 = std::array<long, 3>;
    using spacetime = std::pair<long, point3>;

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

        bool operator==(const hail_init&) const = default;

        point3 operator[](long t) const { return {x0 + t * vx, y0 + t * vy, z0 + t * vz}; }

        hail_init& operator-=(const hail_init& other) {
            x0 -= other.x0;
            y0 -= other.y0;
            z0 -= other.z0;
            vx -= other.vx;
            vy -= other.vy;
            vz -= other.vz;
            return *this;
        }

        hail_init operator-(const hail_init& other) {
            auto x = *this;
            x -= other;
            return x;
        }

        hail_init& operator+=(const hail_init& other) {
            x0 += other.x0;
            y0 += other.y0;
            z0 += other.z0;
            vx += other.vx;
            vy += other.vy;
            vz += other.vz;
            return *this;
        }

        void move1(std::array<long, 3>& pos) const {
            pos[0] += vx;
            pos[1] += vy;
            pos[2] += vz;
        }

        static hail_init from(point3 a, point3 b, bool reduce = true) {
            auto [ax, ay, az] = a;
            auto [bx, by, bz] = b;
            std::array speeds{bx - ax, by - ay, bz - az};
            auto div = reduce ? std::gcd(std::gcd(speeds[0], speeds[1]), speeds[2]) : 1;
            return {ax, ay, az, (bx - ax) / div, (by - ay) / div, (bz - az) / div};
        }

        static std::optional<hail_init> from(spacetime a, spacetime b) {
            long dt = b.first - a.first;
            hail_init to_return{};
            std::array pos{&hail_init::x0, &hail_init::y0, &hail_init::z0};
            std::array vel{&hail_init::vx, &hail_init::vy, &hail_init::vz};

            for (int i = 0; i < 3; ++i) {
                if ((b.second[i] - a.second[i]) % dt != 0)
                    return std::nullopt;
                to_return.*(vel[i]) = (b.second[i] - a.second[i]) / dt;
                to_return.*(pos[i]) = (a.second[i] - a.first * to_return.*(vel[i]));
            }
            return to_return;
        }

        static std::optional<hail_init> from(point3 b, bool reduce = true) { return from({0, 0, 0}, b, reduce); }
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

        static std::optional<line_2d_equation> from(std::pair<long, long> b) { return from({0, 0}, b); }

        static std::optional<line_2d_equation> from(hail_init b) {
            auto [x1, y1, z1] = b[0];
            auto [x2, y2, z2] = b[1];
            return from({x1, y1}, {x2, y2});
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

        static std::optional<dots_2d_equation> from(std::pair<long, long> b) { return from({0, 0}, b); }

        static std::optional<dots_2d_equation> from(hail_init b) {
            auto [x1, y1, z1] = b[0];
            auto [x2, y2, z2] = b[1];
            return from({x1, y1}, {x2, y2});
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

    std::optional<spacetime> intersect_discrete(hail_init l, hail_init r) {
        // Assuming t1 and t2 are diff for intersection
        // return in r's time frame

        // (a, b) + (x, y)t1  == (c, d) + (f, g)t2
        // t2 = (dx - bx - yc + ya) / (yf - xg)

        std::array components{
                std::array{&hail_init::x0, &hail_init::y0, &hail_init::vx, &hail_init::vy},
                std::array{&hail_init::y0, &hail_init::z0, &hail_init::vy, &hail_init::vz},
                std::array{&hail_init::z0, &hail_init::x0, &hail_init::vz, &hail_init::vx},
        };

        for (auto comp : components) {
            auto [a, b, x, y] = comp;
            auto [c, d, f, g] = comp;
            long dx = r.*d * l.*x;
            long bx = l.*b * l.*x;
            long yc = l.*y * r.*c;
            long ya = l.*y * l.*a;
            long yf = l.*y * r.*f;
            long xg = l.*x * r.*g;

            long numerator = (dx - bx - yc + ya);
            long denom = (yf - xg);

            if (denom == 0 or numerator % denom != 0) {
                continue;
            }

            long time_2 = numerator / denom;

            return std::pair{time_2, r[time_2]};
        }
        return std::nullopt;
    }

    STREAM_IN(hail_init, h) {
        std::string s;
        std::getline(in, s);
        sscanf(s.c_str(), "%ld, %ld, %ld @ %ld, %ld, %ld", &h.x0, &h.y0, &h.z0, &h.vx, &h.vy, &h.vz);
        return in;
    }

    STREAM_OUT(hail_init, h) {
        return out << std::format("{}, {}, {} @ {}, {}, {}", h.x0, h.y0, h.z0, h.vx, h.vy, h.vz);
    }

    STREAM_OUT(dots_2d_equation, d) {
        return out << std::format("{}t + {}", d.a, d.b);
    }

    double min_dist_from_0(hail_init h) {
        double t = double(h.x0 * h.vx + h.y0 * h.vy + h.z0 * h.vz) / (h.vx * h.vx + h.vy * h.vy + h.vz * h.vz);
        return t;
    }

    hail_init find_init(std::vector<hail_init>& hail) {
        //        auto ref = hail.back();
        //        hail.pop_back();

        stdr::sort(hail, std::less(), min_dist_from_0);
        auto first = hail.back();
        hail.pop_back();
        auto second = hail.back();
        hail.pop_back();

        std::cout << first << std::endl;

        for (long t = 0x0003fc2600000;; ++t) {
            auto start = first[t];
            //            if ((t & 0xffff) == 0) {
            if ((t & 0xfffff) == 0)
                printf("%013lx\n", t);
            //            }
            //            auto start_stream = hail | stdv::transform([t](const hail_init& h) { return h[t]; });
            //            for (auto start : start_stream) {
            auto eq = hail_init::from(start, true);
            if (!eq)
                continue;

            auto cross = intersect_discrete(*eq, second);

            if (!cross)
                continue;

            auto stone = hail_init::from({t, start}, *cross);

            if (not stone)
                continue;

            for (auto hailstone : hail) {
                auto x1 = hailstone[0];
                auto x2 = hailstone[1];

                auto xEq = hail_init::from(std::pair(0, x1), std::pair(1, x2));

                if (not xEq) {
                    continue;
                }

                if (intersect_discrete(*stone, *xEq).value_or(std::pair{-1, point3{}}).first < 0)
                    goto fail;
            }

            return *stone;
        fail:
            //            }
        }
    };

    stdfuture::generator<std::pair<long, long>> pairs_of_int() {
        int sign[2] = {1, -1};
        for (long y = 1;; ++y) {
            for (long x = 1; x <= y; ++x) {
                for (int sx : sign) {
                    for (int sy : sign) {
                        co_yield {sx * x, sy * y};
                        if (x != y)
                            co_yield {sy * y, sx * x};
                    }
                }
            }
        }
    }

    hail_init find_init2(std::vector<hail_init>& hail) {
        auto first = hail.back();
        hail.pop_back();
        auto second = hail.back();
        hail.pop_back();

        auto [x1, y1, z1, v1, w1, u1] = first;
        auto [x2, y2, z2, v2, w2, u2] = second;

        for (auto [a, b] : pairs_of_int()) {
            auto lambda = (a - v2);
            auto kappa = (b - w2);
        }

        std::cout << first << std::endl;

        long start_time = (long) min_dist_from_0(first);

        for (long t = start_time;; ++t) {
            auto start = first[t];
            //            if ((t & 0xffff) == 0) {
            if ((t & 0xfffff) == 0)
                printf("%013lx\n", t);
            //            }
            //            auto start_stream = hail | stdv::transform([t](const hail_init& h) { return h[t]; });
            //            for (auto start : start_stream) {
            auto eq = hail_init::from(start, true);
            if (!eq)
                continue;

            auto cross = intersect_discrete(*eq, second);

            if (!cross)
                continue;

            auto stone = hail_init::from({t, start}, *cross);

            if (not stone)
                continue;

            for (auto hailstone : hail) {
                auto x1 = hailstone[0];
                auto x2 = hailstone[1];

                auto xEq = hail_init::from(std::pair(0, x1), std::pair(1, x2));

                if (not xEq) {
                    continue;
                }

                if (intersect_discrete(*stone, *xEq).value_or(std::pair{-1, point3{}}).first < 0)
                    goto fail;
            }

            return *stone;
        fail:
            //            }
        }
    };

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
        auto head = hail.front();
//        stdr::for_each(hail, [&head](hail_init& h) { h -= head; });
        auto updated_head = hail.front();
        auto [hx, hy, hz, vhx, vhy, vhz] = updated_head;

        using b = ox::system::builder;

        auto eq1 = b(hx) - b("rx") + (b(vhx) - b("vrx")) * b("t1");
        auto eq2 = b(hy) - b("ry") + (b(vhy) - b("vry")) * b("t1");
        auto zero = b(0);

        auto lhs = eq1.tree_node;
        auto rhs = zero.tree_node;

        ox::system::isolate(lhs, rhs, "t1");
        auto Xt1 = rhs;

        printf("\n\n");
        lhs = eq2.tree_node;
        rhs = zero.tree_node;
        ox::system::isolate(lhs, rhs, "t1");
        auto Yt1 = rhs;

        printf("\n\n");
        lhs = Xt1;
        rhs = Yt1;
        ox::system::isolate(lhs, rhs, "rx");

        return {};

        //        for (const auto& [x, y] : stdv::pairwise(hail)) {
        //            if (x.vx != y.vx)
        //                continue;
        //            x.geogebra_ray_2d();
        //            y.geogebra_ray_2d();
        //            (y - x).geogebra_ray_2d();
        //            std::cout << std::endl;
        //        }
        //
        //        printf("-------------------\n");
        //        stdr::sort(hail, std::less(), &hail_init::vy);
        //        for (const auto& [x, y] : stdv::pairwise(hail)) {
        //            if (x.vy != y.vy)
        //                continue;
        //            x.geogebra_ray_2d();
        //            y.geogebra_ray_2d();
        //            (y - x).geogebra_ray_2d();
        //            std::cout << std::endl;
        //        }
        //
        //        auto uniques = stdr::unique(hail, stdr::equal_to(), &hail_init::vx);
        //        std::cout << stdr::distance(uniques) << std::endl;
        //
        //        stdr::sort(hail, std::less(), &hail_init::vy);
        //        uniques = stdr::unique(hail, stdr::equal_to(), &hail_init::vy);
        //        std::cout << stdr::distance(uniques) << std::endl;
        //
        //        stdr::sort(hail, std::less(), &hail_init::vz);
        //        uniques = stdr::unique(hail, stdr::equal_to(), &hail_init::vz);
        //        std::cout << stdr::distance(uniques) << std::endl;

        //        return {};

        //        auto [hx1, hy1, hz1, vhx1, vhy1, vhz1] = hail[0];
        //        auto [hx2, hy2, hz2, vhx2, vhy2, vhz2] = hail[1];
        //        auto [hx3, hy3, hz3, vhx3, vhy3, vhz3] = hail[2];
        //
        //        auto vrz = ((vhz1 - vhz2) * hy1 * vhz1 - (vhz1 - vhz2) * hy3 * vhz3 - (vhz1 - vhz3) * hy1 * vhz1
        //                    + (vhz1 - vhz3) * hy2 * vhz2)
        //                 / ((vhz1 - vhz3) * (hy2 - hy1) - (vhz1 - vhz2) * (hy3 - hy1));
        //        auto vrx =  ((vhx1 - vhx2) * hy1 * vhx1 - (vhx1 - vhx2) * hy3 * vhx3 - (vhx1 - vhx3) * hy1 * vhx1
        //                    + (vhx1 - vhx3) * hy2 * vhx2)
        //                 / ((vhx1 - vhx3) * (hy2 - hy1) - (vhx1 - vhx2) * (hy3 - hy1));
        //
        //        std::cout << vrz << std::endl << vrx << std::endl;
        //
        //        return {};

        //        auto head = hail.back();
        //        hail.pop_back();
        //        hail_init head{24, 13, 10, -3, 1, 2};

        //        sleep(5);
        //        for (auto h : hail | stdv::take(6)) {
        //            auto [hx1, hy1, hz1, vhx1, vhy1, vhz1] = h;
        //            std::string ray = std::format("Ray(({} - a, {} - b, {} - c), Vector(({} - v, {} - j, {} - k)))\n",
        //                                          (double)hx1 / 1'000'000'000'000,
        //                                          (double)hy1/ 1'000'000'000'000,
        //                                          (double)hz1/ 1'000'000'000'000,
        //                                          (double)vhx1/ 1'000'000'000'000,
        //                                          (double)vhy1/ 1'000'000'000'000,
        //                                          (double)vhz1/ 1'000'000'000'000);
        //            auto command = std::format("xdotool type --window `xdotool getactivewindow` \"{}\"", ray);
        //            system(command.c_str());
        //        }

        //        return {};
        //
        //        auto stone = find_init(hail);
        //        stone += head;
        //
        //        std::cout << stone << std::endl;
        //        return stone.x0 + stone.y0 + stone.z0;
    }
} // namespace aoc2023::day24