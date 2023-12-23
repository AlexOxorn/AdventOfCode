#include "../../../common.h"

#include <queue>
#include <functional>
#include <format>
#include <ox/parser.h>

namespace aoc2023::day20 {
    enum Pulse { Low, High };
    enum NodeType { Sink, Flipflop, Disjoint, Broadcast };

    struct node;
    using pulse_map = std::unordered_map<std::string, node>;

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

    struct input_node : std::pair<std::string, node> {
        using std::pair<std::string, node>::pair;
    };

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

    STREAM_OUT(node, n) {
        switch (n.type) {
            case Broadcast: out << ""; break;
            case Flipflop: out << "%"; break;
            case Disjoint: out << "&"; break;
            case Sink: out << ""; break;
        }
        return out << n.name;
    }

    STREAM_IN(input_node, n) {
        using namespace ox::parser;
        using namespace literals;
        static auto parser = ((("%"_l(read_ff) | "&"_l(read_dis) | "broadcaster"_l(read_bb)) + !String("-", set_name)))
                           + "->"_l + List(",", String(push_dest));

        n.second.dest.clear();
        n.second.name.clear();
        std::string line;
        std::getline(in, line);
        auto res = parser.parse(&n.second, line);
        n.first = n.second.name;
        if (!res) {
            in.setstate(std::ios::failbit);
        }
        return in;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto input = get_stream<input_node>(filename);
        pulse_map map(input.begin(), input.end());
        map["rx"].type = Sink;
        map["rx"].name = "rx";

        for (auto& node : map | stdv::values) {
            for (auto& d : node.dest) {
                map[d].disjoint_memory[node.name] = Low;
            }
        }

        pulse_queue q;
        q.map = std::move(map);
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
        auto input = get_stream<input_node>(filename);
        pulse_map map(input.begin(), input.end());
        map["rx"].type = Sink;
        map["rx"].name = "rx";

        for (auto& node : map | stdv::values) {
            for (auto& d : node.dest) {
                map[d].disjoint_memory[node.name] = Low;
            }
        }

        std::queue<node> dependencies;
        dependencies.push(map["rx"]);
        std::queue<node> dependencies_next;
        long indent = 0;
        for (int i = 0; i < 0; ++i, ++indent, std::swap(dependencies, dependencies_next)) {
            auto p_indent = [indent]() {
                for (char c : stdv::repeat(' ') | stdv::take(indent * 2)) {
                    std::cout << c;
                }
            };

            while (!dependencies.empty()) {
                node top = dependencies.front();
                dependencies.pop();
                p_indent();
                std::cout << top << " depends on ";
                for (auto& name : top.disjoint_memory | stdv::keys) {
                    auto& dop = map[name];
                    std::cout << dop << ", ";
                    dependencies_next.push(dop);
                }
                std::cout << std::endl;
            }
        }

        pulse_queue q;
        q.map = std::move(map);

        /*
rx depends on &th,
&th depends on &qn, &xf, &xn, &zl,
  &qn depends on &vc,
  &xf depends on &db,
  &xn depends on &gf,
  &zl depends on &qx,
    &vc depends on %qk, %vz, %cd, %pm, %sb, %cr, %hd,
    &db depends on %jz, %ch, %mc, %qj, %nn, %pl, %xm, %bp,
    &gf depends on %vl, %zd, %fn, %pr, %qq, %sr, %ln, %tj, %lc, %gm,
    &qx depends on %bx, %rz, %kt, %bf, %cl, %jd, %qp, %pf,
        %qk depends on %pm,
        %vz depends on %ks,
        %cd depends on %gx,
        %pm depends on %cd,
        %sb depends on %lr,
        %cr depends on %vz,
        %hd depends on broadcast, &vc,

        %jz depends on %bp,
        %ch depends on broadcast, &db,
        %mc depends on %ch,
        %qj depends on %jz,
        %nn depends on %cc,
        %pl depends on %ff,
        %xm depends on %qj,
        %bp depends on %sf,

        %vl depends on %sr,
        %zd depends on %fj,
        %fn depends on %lc,
        %pr depends on %fn,
        %qq depends on %ln,
        %sr depends on broadcast, &gf,
        %ln depends on %zd,
        %tj depends on %gm,
        %lc depends on %tj,
        %gm depends on %qm,

        %bx depends on broadcast, &qx,
        %rz depends on %kt,
        %kt depends on %cb,
        %bf depends on %cl,
        %cl depends on %vm,
        %jd depends on %xz,
        %qp depends on %bx,
        %pf depends on %bf,
         */

        long count = 1;
        for (;; ++count) {
            q.emplace("button", "broadcast", Low);
            while (!q.empty()) {
                q.handle();
            }
            if (q.dest_counts["rx"].first == 1) {
                break;
            }
            std::cout << std::format("{}\n", q.dest_counts);
            q.dest_counts.clear();
        }
        printf("%ld\n", count + 1);
        return count;
    }
} // namespace aoc2023::day20