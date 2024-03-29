#include <cassert>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <limits>
#include "../../../common.h"

#define DAY 14

namespace aoc2021::day14 {
    struct insertion_rule : public std::pair<std::string, char> {};

    STREAM_IN(insertion_rule, i) {
        std::string temp;
        in >> i.first;
        in >> temp;
        assert(!in || temp == "->");
        in >> i.second;
        return in;
    }

    class polymer_decoding {
        std::string current_template;
        std::map<std::string, char, std::less<>> insertion_rules;

        std::unordered_map<std::string, long> _multistep_imp(int max) {
            std::unordered_map<std::string, long> pair_count;
            for(auto point = current_template.begin(); point+1 < current_template.end(); ++point) {
                pair_count[std::string(point, point + 2)]++;
            }
            for([[maybe_unused]] int iteration : stdv::iota(0, max)) {
                std::unordered_map<std::string, long> new_pair_count;
                for(auto& [pair, count] : pair_count) {
                    auto rule = insertion_rules.find(pair);
                    if (rule == insertion_rules.end())
                        continue;
                    new_pair_count[std::string({pair[0], rule->second})] += count;
                    new_pair_count[std::string({rule->second, pair[1]})] += count;
                }
                pair_count = std::move(new_pair_count);
            }
            return pair_count;
        }
    public:
        polymer_decoding(ox::ifstream_container<insertion_rule>& in) {
            getline(in, current_template);
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            for (auto& node : in) {
                insertion_rules.insert(node);
            }
        }

        void step() {
            current_template.reserve(current_template.size() * 2);
            for(auto point = current_template.begin(); point+1 <= current_template.end(); ++point) {
                std::string_view part(point, point+2);
                auto rule = insertion_rules.find(part);
                if (rule == insertion_rules.end())
                    continue;
                current_template.insert(++point, rule->second);
            }
        }

        std::unordered_map<char, long> multistep(int max) {
            auto pair_count = _multistep_imp(max);
            std::unordered_map<char, long> character_count;
            character_count[current_template.front()]++;
            character_count[current_template.back()]++;
            for (const auto& [pair, count] : pair_count) {
                character_count[pair[0]] += count;
                character_count[pair[1]] += count;
            }
            for (auto& [_, count] : character_count) {
                count /= 2;
            }
            return character_count;
        };

        const std::string& get_polymer() {
            std::unordered_map<std::string, int> pair_count;
            for(auto point = current_template.begin(); point+1 <= current_template.end(); ++point) {
                pair_count[std::string(point, point + 2)]++;
            }
            return current_template;
        }
    };

    long solve_for(puzzle_options filename, int steps) {
        auto input = get_stream<insertion_rule>(filename);
        polymer_decoding p(input);
        std::unordered_map<char, long> char_count = p.multistep(steps);
        auto counts = char_count | stdv::transform(&std::unordered_map<char, long>::value_type::second);

        auto [min, max] = stdr::minmax(counts);
        myprintf("Min count after %d steps is %ld and the max is %ld\nTheir difference is %ld\n",
               steps, min, max, max - min);
        return max - min;
    }

    answertype puzzle1(puzzle_options filename) {
        return solve_for(filename, 10);
    }

    answertype puzzle2(puzzle_options filename) {
        return solve_for(filename, 40);
    }
}
