#include "../../../common.h"
#include <ranges>
#include <algorithm>
#include <numeric>
#include <thread>
#include <atomic>
#include <ox/parser.h>
#include <ox/math.h>

namespace aoc2023::day12 {

    struct springs {
        std::string blueprint;
        std::vector<int> broken;

        [[nodiscard]] springs unfold() const {
            auto new_blueprint_view = stdv::repeat(blueprint) | stdv::take(5) | stdv::join_with(std::string("?"));
            auto new_broken_view = stdv::repeat(broken) | stdv::take(5) | stdv::join;
            return {
                    std::string{new_blueprint_view.begin(), new_blueprint_view.end()},
                    std::vector(new_broken_view.begin(), new_broken_view.end())
            };
        }

        [[nodiscard]] auto unfold2() const {
            std::pair<std::array<std::string, 3>, std::vector<int>> to_return;
            to_return.first[0] = blueprint;
            std::string ref = std::string("?") + blueprint;
            to_return.first[1] = ref;
            stdr::rotate(ref, ref.begin() + 1);
            to_return.first[2] = ref;
            to_return.second = broken;
            return to_return;
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
    std::istream& operator>>(std::istream& in, springs& s) {
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

    long count_configurations_rec(std::string s, const std::vector<int>& broken, int s_pos, int b_pos) {
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

    long count_configurations(const std::string& s, const std::vector<int>& broken) {
        long res = count_configurations_rec(s, broken, 0, 0);
        myprintf("%s -> %ld\n", s.c_str(), res);
        return res;
    }
    long count_configurations(const std::array<std::string, 3>& s, const std::vector<int>& broken) {
        auto [init, pre, post] = s;
        long res1 = count_configurations(init, broken);

        int last_non_pos = int(init.find_last_of("#?"));
        int last_start_pos = int(init.rfind('.', last_non_pos)) + 1;

        if (last_non_pos - last_start_pos == broken.back()) {
            if (std::any_of(init.begin() + last_start_pos, init.begin() + last_non_pos + 1, [](char c) {
                    return c == '#';
                })) {
                myprintf("%s -> %ld\n", init.c_str(), res1);
                return res1;
            }
        }
        long res2 = count_configurations(pre, broken);
        long res3 = count_configurations(post, broken);
        long res = res1 * ox::fast_pow(std::max(res2, res3), 4u);
        myprintf("%s -> %ld\n", init.c_str(), res);
        return res;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto x = get_stream<springs>(filename);
        auto configs =
                x | stdv::transform([](const springs& s) { return count_configurations(s.blueprint, s.broken); });
        auto res = std::accumulate(configs.begin(), configs.end(), 0l);
        printf("%ld\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto x = get_from_input<springs>(filename);
        auto configs = x | stdv::transform(&springs::unfold);
        std::vector unfolded_springs(configs.begin(), configs.end());
        std::vector<long> results(unfolded_springs.size() * 16);
        std::atomic_int index = 0;
        std::vector<std::thread> threads;
        for (int i = 0; i < int(std::thread::hardware_concurrency()); ++i) {
            threads.emplace_back([&]() {
                while(true) {
                    int i = index.fetch_add(1);
                    if (i >= int(unfolded_springs.size())) {
                        return;
                    }
                    auto& sprg = unfolded_springs[i];
                    results[i * 16] = count_configurations(sprg.blueprint, sprg.broken);
                }
            });
        }
        for (std::thread& t : threads) {
            t.join();
        }
        long res = std::accumulate(results.begin(), results.end(), 0l);

//        for (const auto& [xx, yy] : stdv::zip(x, configs)) {
//            springs new_sping = xx.unfold();
//            long proper = count_configurations(new_sping.blueprint, new_sping.broken);
//            if (proper != yy) {
//                printf("%s | ", xx.blueprint.c_str());
//                for (int i : xx.broken) {
//                    printf("%d, ", i);
//                }
//                printf("\n");
//                long og = count_configurations(xx.blueprint, xx.broken);
//                printf("\t%ld -> %ld\n----------------------\n", og, proper);
//            }
//        }
//        auto res = std::accumulate(configs.begin(), configs.end(), 0l);
        printf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day12