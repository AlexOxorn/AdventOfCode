//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <ranges>
#include <numeric>
#include <deque>
#include <functional>
#include <sstream>
#include <cstring>
#include <algorithm>

namespace aoc2022::day11 {
    struct monkey;
    using troupe = std::vector<monkey>;

    struct monkey {
        static long modulo;

        troupe* group{};
        long divisible_test{};
        int true_monkey{};
        int false_monkey{};
        std::deque<long> worry;
        std::function<long(long)> operation;
        long inspection_count{};

        template <bool calm>
        void round() {
            while (!worry.empty()) {
                long item = worry.front();
                worry.pop_front();
                item = operation(item);
                if constexpr (calm)
                    item /= 3;
                else
                    item %= modulo;
                group->at(item % divisible_test == 0 ? true_monkey : false_monkey).worry.push_back(item);
                inspection_count++;
            }
        }
    };

    long monkey::modulo = 0;

    std::istream& operator>>(std::istream& in, monkey& mon) {
        std::string s;
        long i;
        char c;

        // Monkey ID
        while (std::getline(in, s), !s.starts_with("Monkey") && in)
            ;

        // Starting Items
        std::getline(in, s);
        std::istringstream ss(s);
        ss.ignore(10000, ':');
        mon.worry.clear();
        while (ss >> i) {
            mon.worry.push_back(i);
            ss >> c;
        }

        // Operation
        std::getline(in, s);
        char right[10];
        char op;
        sscanf(s.c_str(), "  Operation: new = old %c %s", &op, right);
        if (strcmp(right, "old") == 0) {
            mon.operation = op == '+' ? [](long old) { return old + old; }
                                      : [](long old) { return old * old; } ;
        } else {
            i = strtol(right, nullptr, 10);
            if (op == '+')
                mon.operation = [=](long old) {
                    return old + i;
                };
            else
                mon.operation = [=](long old) {
                    return old * i;
                };
        }

        // Test
        std::getline(in, s);
        sscanf(s.c_str(), "  Test: divisible by %ld", &mon.divisible_test);

        // True
        std::getline(in, s);
        sscanf(s.c_str(), "    If true: throw to monkey %d", &mon.true_monkey);

        // False
        std::getline(in, s);
        sscanf(s.c_str(), "    If false: throw to monkey %d", &mon.false_monkey);

        return in;
    }

    template <int Iterations, bool calm>
    auto solve(const char* filename) {
        troupe monkeys = get_from_input<monkey>(filename);
        for (monkey& mon : monkeys) {
            mon.group = &monkeys;
        }
        auto divs = monkeys | stdv::transform(&monkey::divisible_test);
        monkey::modulo = std::accumulate(divs.begin(), divs.end(), 1l, std::multiplies<>());
        for (int i = 0; i < Iterations; i++) {
            stdr::for_each(monkeys, &monkey::round<calm>);
        }
        std::vector<long> inspections;
        stdr::transform(monkeys, std::back_inserter(inspections), &monkey::inspection_count);
        stdr::nth_element(inspections, inspections.begin() + 2, std::greater<>());

        myprintf("the product for the two most handsy monkeys are %ld\n", inspections[0] * inspections[1]);
        return inspections[0] * inspections[1];
    }

    answertype puzzle1(const char* filename) {
        return solve<20, true>(filename);
    }

    answertype puzzle2(const char* filename) {
        return solve<10000, false>(filename);
    }
} // namespace aoc2022::day11