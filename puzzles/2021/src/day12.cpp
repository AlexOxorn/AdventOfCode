#include "../../../common.h"

#define DAY 12

#include <vector>
#include <set>
#include <optional>
#include <unordered_map>
#include <ranges>
#include <algorithm>

#define pushpop(p, a, X) \
p.push_back(a);\
X;\
p.pop_back();

namespace aoc2021::day12 {
    class link : public std::pair<std::string, std::string> {};

    STREAM_IN(link, l) {
        std::string s;
        std::getline(in, l.first, '-');
        std::getline(in, l.second);
        return in;
    }

    class graph {
        using path = std::vector<std::string>;
        std::unordered_multimap<std::string, std::string> links;

        void _valid_paths_rec(path& p, std::vector<path>& out, int double_visited = true) {
            std::string& curr = p.back();
            auto [begin, end] = links.equal_range(curr);
            for(const auto &[_, value] : stdr::subrange(begin, end)) {
                if (value == "end") {
                    pushpop(p, value, out.push_back(p))
                } else if (std::isupper(value[0]) || std::find(p.begin(), p.end(), value) == p.end()) {
                    pushpop(p, value, _valid_paths_rec(p, out, double_visited))
                } else if (value != "start" && !double_visited ) {
                    pushpop(p, value, _valid_paths_rec(p, out, true))
                }
            }
        }

    public:
        template<stdr::range R>
        explicit graph(R& r) {
            for (const link& l : r) {
                links.emplace(l.first, l.second);
                links.emplace(l.second, l.first);
            }
        }

        std::vector<path> get_valid_paths(bool with_return = false) {
            path p{"start"};
            p.reserve(links.size() * 3);
            std::vector<path> out;
            _valid_paths_rec(p, out, !with_return);
            return out;
        }
    };

    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<link>(filename);
        graph g(input);
        auto paths = g.get_valid_paths();
        myprintf("Number of unique paths is: %zu\n", paths.size());
        return paths.size();
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<link>(filename);
        graph g(input);
        auto paths = g.get_valid_paths(true);
        myprintf("Number of unique paths is: %zu\n", paths.size());
        return paths.size();
    }
}
