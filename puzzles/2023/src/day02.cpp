#include "../../../common.h"
#include <string_view>
#include <algorithm>
#include <numeric>
#include <ranges>

namespace aoc2023::day02 {
    enum color : int {
        RED,
        GREEN,
        BLUE,
        LAST
    };

    using colors = std::array<int, LAST>;

    struct game {
        int id{};
        std::vector <colors> pulls;
    };

    std::istream& operator>>(std::istream& in, game& g) {
        g.pulls.clear();
        std::string line;
        std::getline(in, line);
        const char* head = line.c_str();
        int push;
        sscanf(head, "Game %d: %n", &g.id, &push);
        head += push;

        colors current{};
        while (head < line.end().base() && *head != '\n') {
            int amount;
            char color[10];
            sscanf(head, "%d %s%n", &amount, color, &push);
            head += push;
            switch (color[0]) {
                case 'b': current[BLUE] = amount; break;
                case 'r': current[RED] = amount; break;
                case 'g': current[GREEN] = amount; break;
                default: break;
            }
            if (std::string_view(color).ends_with(';')) {
                g.pulls.emplace_back(current);
                current = {};
            }
        }
        g.pulls.emplace_back(current);

        return in;
    }

    constexpr int max_count[] {12, 13, 14};

    bool valid_game(const game& g) {
        return stdr::all_of(g.pulls, [](colors x) {
            bool results[3];
            stdr::transform(x, max_count, std::begin(results), std::less_equal<>());
            return stdr::all_of(results, std::identity());
        });
    }

    colors keep_most(colors part, colors n) {
        stdr::transform(part, n, part.begin(), [](int a, int b) { return std::max(a, b); });
        return part;
    }

    long get_power(const game& g) {
        colors max = std::accumulate(g.pulls.begin(), g.pulls.end(), colors{}, keep_most);
        return std::accumulate(max.begin(), max.end(), 1l, std::multiplies<>());
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto games = get_stream<game>(filename);
        auto valid_ids = games | stdv::filter(valid_game) | stdv::transform(&game::id);
        auto sum = std::accumulate(valid_ids.begin(), valid_ids.end(), 0);
        myprintf("%d\n", sum);
        return sum;
    }


    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto games = get_stream<game>(filename);
        auto power = games | stdv::transform(get_power);
        auto sum = std::accumulate(power.begin(), power.end(), 0l);
        myprintf("%ld\n", sum);
        return sum;
    }
} // namespace aoc2023::day02