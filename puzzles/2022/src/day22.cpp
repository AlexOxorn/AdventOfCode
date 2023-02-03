//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <ox/grid.h>

namespace aoc2022::day22 {
    /*
     *  EF
     *  D
     * BC
     * A
     */
    constexpr int cube_width = 50;
    constexpr int Ax = cube_width * 0;
    constexpr int Ay = cube_width * 3;
    constexpr int Bx = cube_width * 0;
    constexpr int By = cube_width * 2;
    constexpr int Cx = cube_width * 1;
    constexpr int Cy = cube_width * 2;
    constexpr int Dx = cube_width * 1;
    constexpr int Dy = cube_width * 1;
    constexpr int Ex = cube_width * 1;
    constexpr int Ey = cube_width * 0;
    constexpr int Fx = cube_width * 2;
    constexpr int Fy = cube_width * 0;
    class map_of_board : public ox::grid<char> {
    public:
        using ox::grid<char>::grid;
        using position = std::optional<const_raw_iterator>;
        typedef position (map_of_board::*move_function)(position) const;
        using pos_dir = std::pair<const_raw_iterator, move_function>;
        typedef pos_dir (map_of_board::*loop_function)(const_raw_iterator, move_function) const;
    private:
        constexpr static move_function up_fn = static_cast<move_function>(&map_of_board::up);
        constexpr static move_function down_fn = static_cast<move_function>(&map_of_board::down);
        constexpr static move_function left_fn = static_cast<move_function>(&map_of_board::left);
        constexpr static move_function right_fn = static_cast<move_function>(&map_of_board::right);

        static move_function opposite_dir(move_function dir) {
            if (dir == up_fn)
                return down_fn;
            if (dir == down_fn)
                return up_fn;
            if (dir == left_fn)
                return right_fn;
            return left_fn;
        }

        static bool within_range(int x, int bound) { return x >= bound and x < bound + cube_width; }

        static bool at_upper_bound(int x, int bound) { return x + 1 == bound + cube_width; }

        static bool at_bound(int x, int bound) { return x == bound; }

        static char dir_char(move_function dir) {
            if (dir == up_fn)
                return '^';
            if (dir == down_fn)
                return 'v';
            if (dir == left_fn)
                return '<';
            return '>';
        }

        [[nodiscard]] const_raw_iterator position_at(int i, int j) const { return data.begin() + i + j * width; }

        [[nodiscard]] pos_dir cube_loop(const_raw_iterator start, move_function dir) const {
            auto [x, y] = coord_from_index(start);

            // Av to F^
            if (within_range(x, Ax) and at_upper_bound(y, Ay) and dir == down_fn)
                return {position_at(x - Ax + Fx, Fy), down_fn};

            // A> to Cv
            if (at_upper_bound(x, Ax) and within_range(y, Ay) and dir == right_fn)
                return {position_at(Cx + y - Ay, Cy + cube_width - 1), up_fn};

            // A< to E^
            if (at_bound(x, Ax) and within_range(y, Ay) and dir == left_fn)
                return {position_at(Ex + y - Ay, Ey), down_fn};

            // B< to E<
            if (at_bound(x, Bx) and within_range(y, By) and dir == left_fn)
                return {position_at(Ex, Ey + cube_width - 1 - (y - By)), right_fn};

            // B^ to D<
            if (within_range(x, Bx) and at_bound(y, By) and dir == up_fn)
                return {position_at(Dx, x - Bx + Dy), right_fn};

            // Cv to A>
            if (within_range(x, Cx) and at_upper_bound(y, Cy) and dir == down_fn)
                return {position_at(Ax + cube_width - 1, x - Cx + Ay), left_fn};

            // C> to F>
            if (at_upper_bound(x, Cx) and within_range(y, Cy) and dir == right_fn)
                return {position_at(Fx + cube_width - 1, Fy + cube_width - 1 - (y - Cy)), left_fn};

            // D< to B^
            if (at_bound(x, Dx) and within_range(y, Dy) and dir == left_fn)
                return {position_at(Bx + y - Dy, By), down_fn};

            // D> to Fv
            if (at_upper_bound(x, Dx) and within_range(y, Dy) and dir == right_fn)
                return {position_at(Fx + y - Dy, Fy + cube_width - 1), up_fn};

            // E^ to A<
            if (within_range(x, Ex) and at_bound(y, Ey) and dir == up_fn)
                return {position_at(Ax, Ay + x - Ex), right_fn};

            // E< to B<
            if (at_bound(x, Ex) and within_range(y, Ey) and dir == left_fn)
                return {position_at(Bx, By + cube_width - 1 - (y - Ey)), right_fn};

            // Fv to D>
            if (within_range(x, Fx) and at_upper_bound(y, Fy) and dir == down_fn)
                return {position_at(Dx + cube_width - 1, Dy + (x - Fx)), left_fn};

            // F^ to Av
            if (within_range(x, Fx) and at_bound(y, Fy) and dir == up_fn)
                return {position_at(x - Fx + Ax, Ay + cube_width - 1), up_fn};

            // F> to C>
            if (at_upper_bound(x, Fx) and within_range(y, Fy) and dir == right_fn)
                return {position_at(Cx + cube_width - 1, Cy + cube_width - 1 - (y - Fy)), left_fn};

            throw std::exception();
        }

        [[nodiscard]] pos_dir flat_loop(const_raw_iterator start, move_function dir) const {
            move_function opposite = opposite_dir(dir);
            position result = start;
            position back = (this->*opposite)(start);
            while (back.has_value() && **back != ' ') {
                result = back;
                back = (this->*opposite)(back);
            }
            return {*result, dir};
        }

        [[nodiscard]] pos_dir move(const const_raw_iterator start, const move_function dir, loop_function loop) const {
            position result = (this->*dir)(start);
            move_function dir_new = dir;
            if (!result || **result == ' ') {
                auto res = (this->*loop)(start, dir);
                result = res.first;
                dir_new = res.second;
            }
            return **result == '#' ? std::pair{start, dir} : std::pair{*result, dir_new};
        }
    public:
        static move_function clockwise(move_function dir) {
            if (dir == up_fn)
                return right_fn;
            if (dir == down_fn)
                return left_fn;
            if (dir == left_fn)
                return up_fn;
            return down_fn;
        }
        static move_function counter(move_function dir) { return opposite_dir(clockwise(dir)); }
        static int dir_value(move_function dir) {
            if (dir == up_fn)
                return 3;
            if (dir == down_fn)
                return 1;
            if (dir == left_fn)
                return 2;
            return 0;
        }

        pos_dir get_start() { return {stdr::find(data, '.'), right_fn}; }

        [[nodiscard]] pos_dir move_2d(const_raw_iterator start, move_function dir) const {
            return move(start, dir, &map_of_board::flat_loop);
        }

        [[nodiscard]] pos_dir move_cube(const_raw_iterator start, move_function dir) const {
            return move(start, dir, &map_of_board::cube_loop);
        }

        void print_state() {
            leveled_foreach([](char c) { printf("%c", c); }, []() { printf("\n"); });
        }

        void print_pos(const_raw_iterator pos, move_function dir) {
            auto [x, y] = coord_from_index(pos);
            printf("\033[%d;%dH%c", y + 1, x + 1, dir_char(dir));
            fflush(stdout);
        }
    };

    std::pair<map_of_board, std::string> get_data(const char* filename) {
        map_of_board map;
        std::string path;
        if (map.get_raw().empty()) {
            std::vector data = get_from_input<ox::line>(filename);
            path = data.back();
            data.pop_back();
            data.pop_back();
            int width = static_cast<int>(stdr::max(data | stdv::transform(&std::string::size)));
            stdr::for_each(data, [width](std::string& s) { s.resize(width, ' '); });
            map = map_of_board{width, data | stdv::join};
        }
        return {map, path};
    }

    template <bool print = false>
    void solve(const char* filename, map_of_board::loop_function func) {
        using namespace std::chrono_literals;
        auto [board, path] = get_data(filename);
        auto [pos, dir] = board.get_start();
        if constexpr (print) {
            printf("\033[H\033[2J");
            board.print_state();
            printf("\033[31m");
            board.print_pos(pos, dir);
        }
        for (const char* head = path.data(); head != path.end().base();) {
            char* new_head;
            long val = strtol(head, &new_head, 10);
            if (new_head == head) {
                dir = (*head++ == 'R' ? map_of_board::clockwise : map_of_board::counter)(dir);
                if constexpr (print)
                    board.print_pos(pos, dir);
                continue;
            }
            head = new_head;
            for (long i = 0; i < val; ++i) {
                auto res = (board.*func)(pos, dir);
                if constexpr (print)
                    if (pos != res.first) {
                        board.print_pos(res.first, res.second);
                        std::this_thread::sleep_for(5ms);
                    }
                pos = res.first;
                dir = res.second;
            }
        }

        auto [x, y] = board.coord_from_index(pos);
        ++x, ++y;
        int rotation_val = map_of_board::dir_value(dir);
        printf("The final x and y position and rotation are %d, %d, %d\n", x, y, rotation_val);
        printf("The result is %d\n", 1000 * y + 4 * x + rotation_val);
    }

    void puzzle1(const char* filename) {
        solve(filename, &map_of_board::move_2d);
    }

    void puzzle2(const char* filename) {
        solve(filename, &map_of_board::move_cube);
    }
} // namespace aoc2022::day22