#include "../../../common.h"

#include <queue>
#include <functional>
#include <format>
#include <ox/parser.h>
#include <ostream>
#include <filesystem>
#include <algorithm>
#include <numeric>

namespace aoc2023::day20 {
    enum Pulse { Low, High };
    enum NodeType { Sink, Flipflop, Disjoint, Broadcast };

    struct node;
    struct pulse_map : std::unordered_map<std::string, node> {
        using std::unordered_map<std::string, node>::unordered_map;
    };

    struct pulse_event {
        std::string source;
        std::string dest;
        Pulse pulse;
    };

    struct pulse_queue : public std::queue<pulse_event> {
        pulse_map map;
        long counts[2] = {};
        bool log = false;
        std::unordered_map<std::string, std::pair<long, long>> dest_counts;
        std::unordered_map<std::string, long> dest_counts_low;

        void handle();
    };

    struct node {
        std::string name;
        std::vector<std::string> dest;
        Pulse memory = Low;
        std::unordered_map<std::string, Pulse> disjoint_memory;
        NodeType type = Sink;

        [[nodiscard]] const char* color_type() const {
            switch (type) {
                case Sink: return "black";
                case Flipflop: return "red";
                case Disjoint: return "blue";
                case Broadcast: return "green";
                default: std::unreachable();
            }
        }

        explicit operator bool() const {
            switch (type) {
                case Flipflop: return memory == High;
                case Disjoint: return stdr::all_of(disjoint_memory | stdv::values, [](Pulse p) { return p == High; });

                default: return false;
            }
        }

        void receive_pulse(pulse_queue& q, const pulse_event& event) {
            Pulse send = Low;
            auto& [from, _, pulse] = event;
            switch (type) {
                case Flipflop:
                    {
                        if (pulse == High)
                            return;
                        memory = memory == High ? Low : High;
                        send = memory;
                        break;
                    }
                case Disjoint:
                    {
                        disjoint_memory[from] = pulse;
                        send = stdr::all_of(disjoint_memory | stdv::values, [](Pulse p) { return p == High; }) ? Low
                                                                                                               : High;
                    }
                    break;
                case Broadcast: break;
                case Sink: return;
            }
            for (const std::string& n : dest) {
                q.emplace(name, n, send);
            };
        }
    };

    void pulse_queue::handle() {
        pulse_event t = front();
        pop();
        (t.pulse == Low ? dest_counts[t.dest].first : dest_counts[t.dest].second)++;
        if (log) {
            myprintf("%s", t.source.c_str());
            myprintf(" -%s-> ", t.pulse == High ? "high" : "low");
            myprintf("%s\n", t.dest.c_str());
        }
        counts[t.pulse]++;
        map[t.dest].receive_pulse(*this, t);
    }

#define NodeTypeCallback(name, type_parsed) \
    std::string_view read_##name(void* ref, std::string_view s) { \
        ((node*) ref)->type = type_parsed; \
        return s; \
    }

    NodeTypeCallback(ff, Flipflop);
    NodeTypeCallback(dis, Disjoint);
    NodeTypeCallback(bb, Broadcast);

    auto push_dest(void* ref, std::string_view s) {
        ((node*) ref)->dest.emplace_back(s);
        return s;
    }

    auto set_name(void* ref, std::string_view s) {
        ((node*) ref)->name = ((node*) ref)->type == Broadcast ? "broadcast" : s;
        return s;
    }

    STREAM_OUT(pulse_map, grid) {
#ifdef __cpp_lib_print
        std::println(out, "digraph G {{\n    node [style=filled]");
        for (const node& n : grid | stdv::values) {
            std::println(out, "    {} [fillcolor={}, color={}]", n.name, n ? "pink" : "white", n.color_type());

            for (const auto& dest : n.dest) {
                std::println(out, "    {} -> {} [color={}]", n.name, dest, n.color_type());
            }
        }
        std::println(out, "}}");
#endif
        return out;
    }

    STREAM_OUT(node, n) {
        switch (n.type) {
            case Broadcast: out << ""; break;
            case Flipflop: out << "%"; break;
            case Disjoint: out << "&"; break;
            case Sink: out << ""; break;
        }
        return out << n.name;
    }

    STREAM_IN(node, n) {
        using namespace ox::parser;
        using namespace literals;
        static auto parser = ((("%"_l(read_ff) | "&"_l(read_dis) | "broadcaster"_l(read_bb)) + !String("-", set_name)))
                           + "->"_l + List(",", String(push_dest));

        n.dest.clear();
        n.name.clear();
        std::string line;
        std::getline(in, line);
        auto res = parser.parse(&n, line);
        if (!res) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    pulse_queue get_data(puzzle_options filename) {
        auto input = get_stream<node>(filename);
        auto key_values = input | stdv::transform([](const node& n) { return std::pair(n.name, n); });
        pulse_map map(key_values.begin(), key_values.end());
        map["rx"].type = Sink;
        map["rx"].name = "rx";

        for (auto& node : map | stdv::values) {
            for (auto& d : node.dest) {
                map[d].disjoint_memory[node.name] = Low;
            }
        }

        pulse_queue q;
        q.map = std::move(map);
        return q;
    }

    void test_loop_points(puzzle_options filename, std::array<long, 4> loops) {
        pulse_queue q = get_data(filename);
        std::fstream out;
        auto generate_file = [&out, &q, filename](long count) {
            std::string text_file_path =
                    std::format("../puzzles/2023/data/day20/source/{:05}{}.txt", count, filename.filename);
            std::string image_file_path =
                    std::format("../puzzles/2023/data/day20/images/{:05}{}.svg", count, filename.filename);
            if (stdfs::exists(image_file_path))
                return 0;
            out.open(text_file_path, std::fstream::out);
            out << q.map;
            out.close();
            std::string command = std::format("dot {} -Tsvg > {}", text_file_path, image_file_path);
            return std::system(command.c_str());
        };

        generate_file(0);

        for (long count = 1;; ++count) {
            q.emplace("button", "broadcast", Low);
            while (!q.empty()) {
                q.handle();
            }
            if (q.dest_counts["rx"].first == 1) {
                break;
            }
            if (stdr::any_of(loops, [count](long l) { return l == count; })) {
                generate_file(count);
            }
            if (count > stdr::max(loops))
                break;
        }
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        pulse_queue q = get_data(filename);
        for (long count = 0; count < 1000; ++count) {
            q.emplace("button", "broadcast", Low);
            while (!q.empty()) {
                q.handle();
            }
        }
        long res = q.counts[0] * q.counts[1];
        printf("low: %ld, high: %ld\n", q.counts[0], q.counts[1]);
        printf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        std::array<long, 4> loop_points{0b111110111011, 0b111101010011, 0b111010011011, 0b111011010001};
        test_loop_points(filename, loop_points);
        long res = std::accumulate(std::begin(loop_points), std::end(loop_points), 1l, std::lcm<long, long>);
        printf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day20