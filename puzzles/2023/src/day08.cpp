#include "../../../common.h"

#include <unordered_map>
#include <unordered_set>
#include <ox/ranges.h>
#include <format>
#include <numeric>
#include <ox/array.h>
#include <ranges>

namespace aoc2023::day08 {
    enum direction { LEFT, RIGHT };

    struct instructions : public std::vector<direction> {
        using std::vector<direction>::vector;
    };

    using node = std::string;
    using destination = std::array<node, 2>;

    struct dessert_map : public std::unordered_map<node, destination> {
        using std::unordered_map<node, destination>::unordered_map;
    };

    dessert_map parse_map(std::istream& in) {
        dessert_map to_return;
        std::string line;
        using chararray = std::array<char, 4>;
        chararray start;
        chararray left;
        chararray right;
        while (std::getline(in, line)) {
            if (line.empty())
                continue;
            sscanf(line.c_str(), "%3s = (%3s, %3s)", start.data(), left.data(), right.data());
            to_return[start.data()] = {left.data(), right.data()};
        }
        return to_return;
    }

    instructions parse_instructions(std::istream& in) {
        char dir;
        instructions to_return;
        while (in.get(dir) && dir != '\n') {
            to_return.push_back(direction(dir == 'R'));
        }
        return to_return;
    }

    void move(direction dir, const dessert_map& map, node& curr) {
        auto loc = map.find(curr);
        if (loc == map.end()) {
            myprintf("%s\n", curr.c_str());
            std::unreachable();
        }
        curr = loc->second[dir];
    }

    using final_options = std::vector<long>;
    std::tuple<long, long, final_options> calculate_loop_length(node start, const instructions& inst,
                                                                const dessert_map& map) {
        std::vector<node> visited;
        long dir = 0;
        final_options finals;
        int found = 0;
        const int tofind = 1;
        while (true) {
            if (dir >= long(inst.size()))
                dir = 0;
            if (start.ends_with('Z')) {
                finals.push_back(long(visited.size()));
            }
            if (auto pos = stdr::find(visited, start); start.ends_with('Z') && pos != visited.end()) {
                if (++found >= tofind)
                    break;
            }
            visited.push_back(start);
            move(inst[dir], map, start);
            ++dir;
        }
        
        auto x = stdr::find(visited, start) - visited.begin();
        return {x, visited.size() - x, finals};
    }
    
    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto in = get_stream(filename);
        instructions inst = parse_instructions(in);
        dessert_map map = parse_map(in);

        node start = "AAA";
        long count = 0;
        for (direction dir : inst | ox::ranges::views::repeat()) {
            move(dir, map, start);
            ++count;
            if (start == "ZZZ") {
                break;
            }
        }

        myprintf("%ld\n", count);
        return count;
    }

    

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto in = get_stream(filename);
        instructions inst = parse_instructions(in);
        dessert_map map = parse_map(in);

        auto starting_points_stream = map | stdv::keys | stdv::filter([](const node& s) { return s.ends_with('A'); });

        auto start = *starting_points_stream.begin();
        std::unordered_set<std::string> visited;

        std::vector<long> loop_starts;
        std::vector<long> loop_length_mod;
        for (auto& s : starting_points_stream) {
            auto [loop_start, loop_length, finals] = calculate_loop_length(s, inst, map);
            myprintf("loop start %1zu => loop length %4zu\n\t", loop_start, loop_length);
            for (long i : finals)
                myprintf("%6zu, ", i);
            myprintf("\n\t");
            std::vector<long> finals_part;
            std::transform(
                    finals.begin() + 1, finals.end(), finals.begin(), std::back_inserter(finals_part), std::minus<>());
            loop_length_mod.push_back(finals_part.front());
            for (long i : finals_part)
                myprintf("%6zu, ", i);
            myprintf("\n");
        }

        auto res = std::accumulate(loop_length_mod.begin(), loop_length_mod.end(), 1l, std::lcm<long, long>);
        myprintf("%zu\n", res);
        return res;
    }
} // namespace aoc2023::day08