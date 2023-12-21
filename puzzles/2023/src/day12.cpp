#include "../../../common.h"
#include <ranges>
#include <algorithm>
#include <numeric>
#include <thread>
#include <unordered_set>
#include <ox/parser.h>

namespace aoc2023::day12 {

    struct springs {
        std::string blueprint;
        std::vector<int> broken;

        [[nodiscard]] springs unfold() const {
#ifdef __cpp_lib_ranges_join_with
            auto new_blueprint_view = stdv::repeat(blueprint) | stdv::take(5) | stdv::join_with(std::string("?"));
#else
            std::string new_blueprint_view;
            for (int i = 0; i < 4; i++) {
                new_blueprint_view += blueprint;
                new_blueprint_view += "?";
            }
            new_blueprint_view += blueprint;
#endif
            auto new_broken_view = stdv::repeat(broken) | stdv::take(5) | stdv::join;
            return {
                    std::string{new_blueprint_view.begin(), new_blueprint_view.end()},
                    std::vector(new_broken_view.begin(), new_broken_view.end())
            };
        }
    };

    struct NFA {
        struct state {
            state* dot{};
            state* hash{};
            bool valid{};
        };

        std::vector<state> states;

        explicit NFA(const std::vector<int>& broken_pattern) :
                states(std::accumulate(broken_pattern.begin(), broken_pattern.end(), 0) + int(broken_pattern.size())
                       + 1) {
            states[0].dot = &states[0];
            states[0].hash = &states[1];

            int i = 1;
            for (auto& b : broken_pattern) {
                for (int j = 0; j < b - 1; ++i, ++j) {
                    states[i].hash = &states[i + 1];
                }
                states[i].dot = &states[i + 1];
                ++i;
                states[i].dot = &states[i];
                if (i + 1 < int(states.size()))
                    states[i].hash = &states[i + 1];
                ++i;
            }

            states[states.size() - 1].valid = true;
            states[states.size() - 2].valid = true;
        }

        [[nodiscard]] auto count(const std::string& match) const {
            using maptype = std::unordered_map<const state*, size_t>;
            maptype curr{
                    {&states[0], 1zu}
            };
            for (char c : match) {
                maptype next;
                next.reserve(states.size());
                for (auto& [key, value] : curr) {
                    if ((c == '.' || c == '?') && key->dot)
                        next[key->dot] += value;
                    if ((c == '#' || c == '?') && key->hash)
                        next[key->hash] += value;
                }
                curr = std::move(next);
            }
            auto x = std::accumulate(curr.begin(), curr.end(), 0zu, [](size_t res, const maptype::value_type& m) {
                return res + (m.first->valid ? m.second : 0zu);
            });
            return x;
        }
    };

    long broken_pipe_callback(void* ref, long l) {
        ((springs*) ref)->broken.push_back(int(l));
        return l;
    }
    std::string_view blueprint_callback(void* ref, std::string_view str) {
        ((springs*) ref)->blueprint = str;
        return str;
    }
    STREAM_IN(springs, s) {
        using namespace ox::parser::literals;
        using namespace ox::parser;
        static auto parser = String(blueprint_callback) + List(",", Int(broken_pipe_callback));

        std::string line;
        std::getline(in, line);
        s.broken.clear();
        auto res = parser.parse(&s, line);
        if (!res) {
            in.setstate(std::ios::failbit);
            return in;
        }
        return in;
    }


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    [[deprecated("Old Solution")]] long count_configurations_rec(std::string s, const std::vector<int>& broken,
                                                                 int s_pos, int b_pos) {
        if (b_pos >= int(broken.size())) {
            return std::none_of(s.begin() + long(s_pos), s.end(), [](char c) { return c == '#'; });
        }
        auto first = s.find_first_of("#?", s_pos);
        if (first == std::string::npos) {
            return 0;
        }
        if (s[first] == '#') {
            if (int(s.size() - first) < broken[b_pos])
                return 0;
            for (int i = 1; i < broken[b_pos]; ++i) {
                switch (s[first + i]) {
                    case '#': break;
                    case '?': s[first + i] = '#'; break;
                    case '.': return 0;
                }
            }
            if (first + broken[b_pos] >= s.size()) {
                return b_pos == int(broken.size()) - 1;
            }

            if (s.at(first + broken[b_pos]) == '#') {
                return 0;
            }
            s.at(first + broken[b_pos]) = '.';
            return count_configurations_rec(s, broken, int(first) + broken[b_pos], b_pos + 1);
        }

        long result = 0;
        s[first] = '#';
        result += count_configurations_rec(s, broken, int(first), b_pos);
        s[first] = '.';
        result += count_configurations_rec(s, broken, int(first), b_pos);
        return result;
    }

    [[deprecated("Old Solution")]] long count_configurations(const std::string& s, const std::vector<int>& broken) {
        long res = count_configurations_rec(s, broken, 0, 0);
        myprintf("%s -> %ld\n", s.c_str(), res);
        return res;
    }
#pragma GCC diagnostic pop

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto x = get_stream<springs>(filename);
        auto configs = x | stdv::transform([](const springs& s) { return std::pair(s.blueprint, NFA(s.broken)); })
                     | stdv::transform([](const std::pair<std::string, NFA>& p) { return p.second.count(p.first); });
        auto res = std::accumulate(configs.begin(), configs.end(), 0zu);
        myprintf("%zu\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto x = get_stream<springs>(filename);
        auto configs = x | stdv::transform(&springs::unfold)
                     | stdv::transform([](const springs& s) { return std::pair(s.blueprint, NFA(s.broken)); })
                     | stdv::transform([](const std::pair<std::string, NFA>& p) { return p.second.count(p.first); });
        auto res = std::accumulate(configs.begin(), configs.end(), 0zu);
        myprintf("%zu\n", res);
        return res;
    }
} // namespace aoc2023::day12