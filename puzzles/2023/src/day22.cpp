#include "../../../common.h"
#include <algorithm>
#include <unordered_set>
#include <format>

namespace aoc2023::day22 {
    bool simple_intercept(long x0, long x1, long y0, long y1) {
        return x0 <= y1 && y0 <= x1;
    }

    struct block {
        long x0, y0, z0;
        long x1, y1, z1;

        bool operator==(const block&) const = default;
        [[nodiscard]] bool intercept(block other) const {
            return simple_intercept(x0, x1, other.x0, other.x1) and simple_intercept(y0, y1, other.y0, other.y1)
               and simple_intercept(z0, z1, other.z0, other.z1);
        }
    };

    STREAM_IN(block, b) {
        char comma, tild;
        return in >> b.x0 >> comma >> b.y0 >> comma >> b.z0 >> tild >> b.x1 >> comma >> b.y1 >> comma >> b.z1;
    }

    struct graph_node {
        block b;
        std::vector<long> to;
        std::vector<long> from;

        [[nodiscard]] bool safe(const std::vector<graph_node>& nodes) const {
            return stdr::none_of(to, [&](long t) { return nodes[t].from.size() == 1; });
        }

        long disappear(std::vector<graph_node>& nodes, long curr) const {
            long chain = 1;
            for (long t : to) {
                auto& from = nodes[t].from;

                if (auto to_rem = stdr::find(from, curr); to_rem != from.end())
                    from.erase(to_rem);
                if (from.empty()) {
                    chain += nodes[t].disappear(nodes, t);
                }
            }
            return chain;
        }
    };

    std::vector<graph_node> calculate_nodes(puzzle_options filename) {
        std::vector falling_blocks = get_from_input<block>(filename);
        stdr::sort(falling_blocks, {}, &block::z0);
        std::vector<graph_node> nodes;
        for (block b : falling_blocks) {
            std::vector<long> its;
            for (; b.z0 > 0; --b.z0, --b.z1) {
                auto x = stdv::iota(0zu, nodes.size())
                       | stdv::filter([b, &nodes](size_t i) { return b.intercept(nodes[i].b); });
                its.assign(x.begin(), x.end());
                if (!its.empty()) {
                    break;
                };
            }
            ++b.z0;
            ++b.z1;
            graph_node to_add(b);
            if (b.z0 != 1) {
                for (long inter : its) {
                    to_add.from.push_back(inter);
                    nodes[inter].to.push_back(long(nodes.size()));
                }
            }
            nodes.push_back(to_add);
        }

        return nodes;
    }

    const std::vector<graph_node>& get_nodes(puzzle_options filename) {
        static std::vector<graph_node> nodes = calculate_nodes(filename);
        return nodes;
    };

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        const std::vector<graph_node>& nodes = get_nodes(filename);
        auto res = stdr::count_if(nodes, [&](const graph_node& gn) { return gn.safe(nodes); });
        myprintf("%zu\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        const std::vector<graph_node>& nodes = get_nodes(filename);
        long index = 0;
        long res = 0;
        for (auto& node : nodes) {
            auto copy = nodes;
            res += node.disappear(copy, index++)- 1;
        }
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day22