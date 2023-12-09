#include "../../../common.h"
#include <sstream>
#include <numeric>
#include <algorithm>

namespace aoc2023::day09 {
    struct pattern : std::vector<long> {
        using std::vector<long>::vector;
    };

    static auto const_back = static_cast<typename pattern::const_reference (pattern::*)(void) const>(&pattern::back);
    static auto const_front = static_cast<typename pattern::const_reference (pattern::*)(void) const>(&pattern::front);

    std::istream& operator>>(std::istream& in, pattern& p) {
        p.clear();
        std::string line;
        std::getline(in, line);
        std::stringstream ss(line);
        p.assign(std::istream_iterator<long>(ss), std::istream_iterator<long>());
        return in;
    }

    std::vector<pattern> difference_tree(const pattern& p) {
        std::vector<pattern> pp{p};
        while (stdr::any_of(pp.back(), [](long l) { return l != 0; })) {
            pattern next(pp.back().size());
            std::adjacent_difference(pp.back().begin(), pp.back().end(), next.begin());
            next.erase(next.begin());
            pp.emplace_back(std::move(next));
        }
        return pp;
    }

    long find_next(const std::vector<pattern>& pp) {
        auto backs = pp | stdv::reverse | stdv::transform(const_back);
        return std::accumulate(backs.begin(), backs.end(), 0l);
    }

    long find_prev(const std::vector<pattern>& pp) {
        auto fronts = pp | stdv::reverse | stdv::transform(const_front);
        return std::accumulate(fronts.begin(), fronts.end(), 0l, [](long part, long n) { return n - part; });
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto patterns = get_stream<pattern>(filename);
        auto predictions = patterns | stdv::transform(difference_tree) | stdv::transform(find_next);
        auto res = std::accumulate(predictions.begin(), predictions.end(), 0l);
        printf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto patterns = get_stream<pattern>(filename);
        auto predictions = patterns | stdv::transform(difference_tree) | stdv::transform(find_prev);
        auto res = std::accumulate(predictions.begin(), predictions.end(), 0l);
        printf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day09