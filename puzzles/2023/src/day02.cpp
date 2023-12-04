#include "../../../common.h"
#include <string_view>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <ox/parser.h>

namespace aoc2023::day02 {
    enum color : int { RED, GREEN, BLUE, LAST };

    using colors = std::array<int, LAST>;

    struct game {
        int id{};
        std::vector<colors> pulls;
    };

    ox::parser::Combination getLineParser() {
        using namespace ox::parser::literals;
        using namespace ox::parser;

        auto assignID = [](void* ref, long l) {
            return ((game*) ref)->id = int(l);
        };

        auto pushVector = [](void* ref) {
            return ((game*) ref)->pulls.emplace_back();
        };

        auto assignColor = [](void* ref, const std::vector<std::any>& parts) {
            auto [amount, color] = parse_any_vector<long, std::string_view>(parts);
            game& g = *((game*) ref);

            switch (color[0]) {
                case 'b': g.pulls.back()[BLUE] = amount; break;
                case 'r': g.pulls.back()[RED] = amount; break;
                case 'g': g.pulls.back()[GREEN] = amount; break;
                default: break;
            }
            return nullptr;
        };

        return "Game"_l + Int(assignID) + ": "_l
             + List(";", List(",", Combination(assignColor, Int(), ("red"_l | "blue"_l | "green"_l))), pushVector);
    }

    std::istream& operator>>(std::istream& in, game& g) {
        static auto Parser = getLineParser();
        g.pulls.clear();
        std::string line;
        std::getline(in, line);

        auto x = Parser.parse(&g, line);
        if (!x)
            in.setstate(std::ios::failbit);

        return in;
    }

    constexpr int max_count[]{12, 13, 14};

    bool valid_game(const game& g) {
        return stdr::all_of(g.pulls, [](colors x) {
#ifdef __cpp_lib_ranges_zip
            auto valid = stdv::zip_transform(std::less_equal<>(), x, max_count);
            return stdr::all_of(valid, std::identity());
#else
                    bool results[3];
                    stdr::transform(x, max_count, std::begin(results), std::less_equal<>());
                    return stdr::all_of(results, std::identity());
#endif
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