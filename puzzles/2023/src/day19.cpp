#include "../../../common.h"
#include <ox/parser.h>
#include <cstring>
#include <numeric>

namespace aoc2023::day19 {
    // =====================
    // TYPES
    // =====================
    struct part {
        long extreme, musical, aero, shiny;

        [[nodiscard]] long score() const { return extreme + musical + aero + shiny; }
    };

    using quality_range = std::pair<long, long>;
    struct parts_range {
        quality_range extreme, musical, aero, shiny;

        parts_range() : extreme{1, 4000}, musical{1, 4000}, aero{1, 4000}, shiny{1, 4000} {}

        [[nodiscard]] parts_range split(quality_range parts_range::*qual, bool less, long amount,
                                        bool exclusive) const {
            auto to_return = *this;
            if (less) {
                (to_return.*qual).second = std::min((to_return.*qual).second, amount - exclusive);
            } else {
                (to_return.*qual).first = std::max((to_return.*qual).first, amount + exclusive);
            }

            return to_return;
        }

        [[nodiscard]] bool valid() const {
            return extreme.first <= extreme.second and musical.first <= musical.second and aero.first <= aero.second
               and shiny.first <= shiny.second;
        }

        [[nodiscard]] long score() const {
            if (!valid()) {
                return 0;
            }
            return (extreme.second - extreme.first + 1) * (musical.second - musical.first + 1)
                 * (aero.second - aero.first + 1) * (shiny.second - shiny.first + 1);
        }
    };

    struct rule {
        int64_t value{};
        char dest[4]{};
        char measure{};
        char op{};
        char _padding[2]{};

        [[nodiscard]] std::function<bool(long, long)> comp() const {
            if (op == '>') {
                return std::greater();
            }
            return std::less();
        }

        [[nodiscard]] long part::*quality() const {
            switch (measure) {
                case 'x': return &part::extreme;
                case 'm': return &part::musical;
                case 'a': return &part::aero;
                case 's': return &part::shiny;
                default: std::unreachable();
            }
        }

        [[nodiscard]] quality_range parts_range::*quality_range_member() const {
            switch (measure) {
                case 'x': return &parts_range::extreme;
                case 'm': return &parts_range::musical;
                case 'a': return &parts_range::aero;
                case 's': return &parts_range::shiny;
                default: std::unreachable();
            }
        }

        [[nodiscard]] bool evaluate(part p) const {
            if (not op) {
                return true;
            }
            return comp()(p.*(quality()), value);
        }

        [[nodiscard]] std::pair<parts_range, parts_range> evaluate(parts_range pp) const {
            return {
                    pp.split(quality_range_member(), op == '<', value, true),
                    pp.split(quality_range_member(), op != '<', value, false),
            };
        }
    };

    struct workflow {
        char name[4]{};
        std::vector<rule> rules;
    };

    STREAM_OUT(rule, w) {
        if (w.op) {
            return out << w.measure << w.op << w.value << ":" << w.dest;
        }
        return out << w.dest;
    }
    STREAM_OUT(workflow, w) {
        out << w.name << "{";
        for (auto& x : w.rules) {
            out << x << ",";
        }
        return out << "}" << std::endl;
    }
    STREAM_OUT(part, w) {
        return out << "{x=" << w.extreme << ",m=" << w.musical << ",a=" << w.aero << ",s=" << w.shiny << "}\n";
    }

    // =====================
    // PARSING
    // =====================
    long assign_rule_value(void* ref, long l) {
        return ((workflow*) ref)->rules.back().value = int64_t(l);
    }
    std::string_view assign_measure_value(void* ref, std::string_view s) {
        ((workflow*) ref)->rules.back().measure = s[0];
        return s;
    }
    std::string_view assign_op_value(void* ref, std::string_view s) {
        ((workflow*) ref)->rules.back().op = s[0];
        return s;
    }
    std::string_view assign_dest_value(void* ref, std::string_view s) {
        auto max = std::min(3zu, s.size());
        strncpy(((workflow*) ref)->rules.back().dest, s.data(), max);
        ((workflow*) ref)->rules.back().dest[max] = '\0';
        return s;
    }
    std::string_view assign_name_value(void* ref, std::string_view s) {
        auto max = std::min(3zu, s.size());
        strncpy(((workflow*) ref)->name, s.data(), max);
        ((workflow*) ref)->name[max] = '\0';
        return s;
    }
    auto push_new_rule(void* ref) {
        return ((workflow*) ref)->rules.emplace_back();
    }

    auto get_workflow_parser() {
        using namespace ox::parser;
        using namespace literals;

        auto measure = ("a"_l(assign_measure_value) | "m"_l(assign_measure_value) | "s"_l(assign_measure_value)
                        | "x"_l(assign_measure_value));
        auto op = (">"_l(assign_op_value) | "<"_l(assign_op_value));

        auto full = std::move(measure) + std::move(op) + Int(assign_rule_value) + ":"_l + String(assign_dest_value);
        auto rule = std::move(full) | String("}", assign_dest_value);

        return String("{", assign_name_value) + "{"_l + List(",", std::move(rule), push_new_rule) + "}"_l;
    }

    STREAM_IN(workflow, w) {
        w.rules.clear();
        static auto parser = get_workflow_parser();
        std::string s;
        std::getline(in, s);

        if (s.empty()) {
            in.setstate(std::ios::eofbit);
            in.setstate(std::ios::failbit);
            return in;
        }

        auto res = parser.parse(&w, s);
        if (!res) {
            in.setstate(std::ios::failbit);
            return in;
        }
        return in;
    }
    STREAM_IN(part, p) {
        char quality;
        char dummy;
        long l;
        in >> dummy;
        while (in && dummy != '}') {
            in >> quality >> dummy >> l >> dummy;
            switch (quality) {
                case 'x': p.extreme = l; break;
                case 'm': p.musical = l; break;
                case 'a': p.aero = l; break;
                case 's': p.shiny = l; break;
                default: std::unreachable();
            }
        }
        return in;
    }

    // =====================
    // EVALUATING
    // =====================
    using rules_map = std::unordered_map<std::string, std::vector<rule>>;

    bool accepted(const rules_map& map, part p, const std::string& rule_name = "in") {
        using namespace std::string_view_literals;
        auto rules = map.at(rule_name);
        for (rule r : rules) {
            if (r.evaluate(p)) {
                if ("A"sv == r.dest)
                    return true;
                if ("R"sv == r.dest)
                    return false;
                return accepted(map, p, r.dest);
            }
        }
        std::unreachable();
    }

    long possible_values(const rules_map& map, parts_range pp, const std::string& rule_name = "in") {
        if (rule_name == "A")
            return pp.score();
        if (rule_name == "R")
            return 0;

        long res = 0l;
        auto rules = map.at(rule_name);
        for (rule r : rules) {
            if (!r.op) {
                return res + possible_values(map, pp, r.dest);
            }
            auto [pass, fails] = r.evaluate(pp);
            res += possible_values(map, pass, r.dest);
            pp = fails;
        }
        return res;
    }

    // =====================
    // MAIN
    // =====================
    auto data(puzzle_options filename) {
        auto in = get_stream(filename);
        rules_map rules;
        std::vector workflows(std::istream_iterator<workflow>{in}, std::istream_iterator<workflow>());
        in.clear();
        std::vector parts(std::istream_iterator<part>{in}, std::istream_iterator<part>());

        for (auto& w : workflows) {
            rules[w.name] = w.rules;
        }

        return std::pair{rules, parts};
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto [rules, parts] = data(filename);
        auto accepted_parts =
                parts | stdv::filter([&](part p) { return accepted(rules, p); }) | stdv::transform(&part::score);
        long res = std::accumulate(accepted_parts.begin(), accepted_parts.end(), 0l);
        myprintf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto [rules, parts] = data(filename);
        long res = possible_values(rules, parts_range{});
        myprintf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day19