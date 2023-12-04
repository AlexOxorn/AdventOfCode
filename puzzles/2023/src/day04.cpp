#include "../../../common.h"

#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <ranges>
#include <ox/parser.h>
#include <ox/math.h>

namespace aoc2023::day04 {
    struct scratchcard {
        int id{};
        std::vector<std::vector<int>> games;

        [[nodiscard]] size_t get_correct_guesses() const {
            std::vector<int> to_return;
            stdr::set_intersection(games[0], games[1], std::back_inserter(to_return));
            return to_return.size();
        }

        static long get_score(size_t count) {
            if (count == 0)
                return 0l;
            return ox::fast_pow(2l, count - 1);
        }
    };

    auto get_parser() {
        using namespace ox::parser::literals;
        using namespace ox::parser;

        auto assignID = [](void* ref, long l) {
            return ((scratchcard*) ref)->id = int(l);
        };

        auto pushVector = [](void* ref) {
            return ((scratchcard*) ref)->games.emplace_back();
        };

        auto addScratch = [](void* ref, long i) {
            ((scratchcard*) ref)->games.back().push_back(i);
            return i;
        };

        return "Card"_l + Int(assignID) + ": "_l
             + List("|", List(Int(addScratch)), pushVector);
    }

    std::istream& operator>>(std::istream& in, scratchcard& s) {
        std::string line;
        std::getline(in, line);
        s.games.clear();
        static auto parser = get_parser();
        auto res = parser.parse(&s, line);
        if (!res) {
            in.setstate(std::ios::failbit);
            return in;
        }
        stdr::sort(s.games[0]);
        stdr::sort(s.games[1]);
        return in;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto cards = get_stream<scratchcard>(filename);
        auto points =
                cards | stdv::transform(&scratchcard::get_correct_guesses) | stdv::transform(&scratchcard::get_score);
        auto total = std::accumulate(points.begin(), points.end(), 0l);
        printf("%ld\n", total);
        return total;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto cards = get_from_input<scratchcard>(filename);
        std::vector<long> copies_list(cards.size());
        stdr::fill(copies_list, 1);

        for (int i = 0; size_t(i) < cards.size(); ++i) {
            const scratchcard& card = cards[i];
            size_t matches = card.get_correct_guesses();
            long copies = copies_list[i];
            for (int j = 0; size_t(j) < matches; ++j) {
                if (i + j + 1 < int(copies_list.size()))
                    copies_list[i + j + 1] += copies;
            }
        }

        long total_copies = std::accumulate(copies_list.begin(), copies_list.end(), 0l);
        printf("%ld\n", total_copies);
        return total_copies;
    }
} // namespace aoc2023::day04