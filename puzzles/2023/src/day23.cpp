#include "../../../common.h"
#include <ox/grid.h>
#include <unordered_set>
#include <ox/hash.h>

namespace aoc2023::day23 {
    struct raw_map : ox::grid<char> {
        using ox::grid<char>::grid;
        void print() {
            leveled_foreach([](char c) { putc(c, stdout); }, []() { putc('\n', stdout); });
        }
        void print(const auto& set) {
            leveled_iterators(
                    [&set](raw_map::const_raw_iterator it) {
                        if (set.contains(it)) {
                            myprintf("\033[31m%c\033[0m", *it);
                        } else {
                            myprintf("%c", *it);
                        };
                    },
                    []() { myprintf("\n"); });
        }
    };

    using coord = std::array<long, 2>;
    using position = raw_map::const_raw_iterator;
    using visited_set = std::unordered_set<position, ox::trivial_hash>;
    using visited_set2 = std::unordered_set<coord, ox::trivial_hash>;
    using dfs_result = std::pair<long, visited_set>;


    struct graph_node {
        coord pos;
        std::vector<std::pair<long, long>> to;
        bool operator==(const graph_node& other) const { return pos == other.pos; }
    };

    using node_list = std::vector<graph_node>;

    node_list make_graph(raw_map& map) {
        auto start = stdr::find(map.get_raw(), '.');
        auto end = stdr::find(map.get_raw() | stdv::reverse, '.').base() - 1;
        auto is_not_wall = [](position x) {
            return *x != '#';
        };

        std::unordered_set<coord, ox::trivial_hash> seen;
        node_list to_return;
        std::deque<std::pair<position, long>> queue;
        queue.emplace_back(start, -1l);

        while (!queue.empty()) {
            auto [top, source] = queue.front();
            queue.pop_front();

            auto coord = map.coord_from_index(top);
            if (seen.contains(coord))
                continue;

            to_return.emplace_back(coord);
            seen.insert(coord);

            auto neighbour_paths = map.cardinal_neighbour_range(top);
            for (auto dir : neighbour_paths | raw_map::const_valid_index() | stdv::filter(is_not_wall)) {
                auto prev = top;
                auto curr = dir;
                for (long i = 1;; ++i) {
                    auto neighbour_paths = map.cardinal_neighbour_range(curr);
                    auto next_rng = (neighbour_paths | raw_map::const_valid_index() | stdv::filter(is_not_wall)
                                     | stdv::filter([prev](auto it) { return it != prev; }));
                    std::vector next(next_rng.begin(), next_rng.end());

                    if (next.empty() && !(curr == start or curr == end))
                        break;
                    if (next.size() > 1zu or curr == start or curr == end) {
                        auto new_coord = map.coord_from_index(curr);
                        if (seen.contains(new_coord)) {
                            auto node = stdr::find(to_return, new_coord, &graph_node::pos);
                            node->to.emplace_back(long(to_return.size()) - 1, i);
                            to_return.back().to.emplace_back(node - to_return.begin(), i);
                        } else {
                            queue.emplace_back(curr, long(to_return.size()) - 1);
                        }
                        break;
                    } else {
                        prev = curr;
                        curr = next.back();
                    }
                }
            }
        }
        return to_return;
    }

    dfs_result max_dfs(raw_map& source, position start, position end, visited_set& visited, long depth = 0) {
        if (visited.contains(start))
            return {0, {}};
        if (start == end) {
            return {depth, visited};
        }
        dfs_result res;

        visited.insert(start);
        if (*start != '.') {
            switch (*start) {
                case '>': res = max_dfs(source, *source.right(start), end, visited, depth + 1); break;
                case 'v': res = max_dfs(source, *source.down(start), end, visited, depth + 1); break;
                case '^': res = max_dfs(source, *source.up(start), end, visited, depth + 1); break;
                case '<': res = max_dfs(source, *source.left(start), end, visited, depth + 1); break;
                default: std::unreachable();
            }
        } else {
            auto neighbours = source.cardinal_neighbour_range(start);
            for (auto next : neighbours | raw_map::const_valid_index() | stdv::filter([](position x) {
                                 return *x != '#';
                             }) | stdv::filter([&visited](position x) { return !visited.contains(x); })) {
                res = stdr::max(res, max_dfs(source, next, end, visited, depth + 1), {}, &dfs_result::first);
            }
        }
        visited.erase(start);
        return res;
    };

    long max_dfs2(const node_list& source, const graph_node& start, const graph_node& end, visited_set2& visited,
                  long depth = 0) {
        if (visited.contains(start.pos))
            return 0l;
        if (start == end) {
            return depth;
        }
        long res = 0;

        visited.insert(start.pos);

        auto neighbours =
                start.to
                | stdv::transform([&](std::pair<long, long> l) { return std::pair(source[l.first], l.second); })
                | stdv::filter([&](const std::pair<graph_node, long>& x) { return !visited.contains(x.first.pos); });
        for (auto [next, length] : neighbours) {
            res = stdr::max(res, max_dfs2(source, next, end, visited, depth + length));
        }

        visited.erase(start.pos);
        return res;
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        raw_map m(get_stream(filename));
        position start = stdr::find(m.get_raw(), '.');
        position end = stdr::find(m.get_raw() | stdv::reverse, '.').base() - 1;
        visited_set v;
        auto [res, path] = max_dfs(m, start, end, v);
        m.print(path);
        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        raw_map m(get_stream(filename));
        auto x = make_graph(m);
        auto start = *stdr::find(x, std::array{1l, 0l}, &graph_node::pos);
        auto end = *stdr::find(x, std::array{m.dimensions[0] - 2, m.dimensions[1] - 1}, &graph_node::pos);
        visited_set2 v;
        auto res = max_dfs2(x, start, end, v);
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day23