#include "../../../common.h"
#include <cctype>
#include <numeric>
#include <string_view>
#include <algorithm>
#include <concepts>

namespace aoc2023::day01 {
    using namespace std::string_view_literals;
    constexpr static const char* digitsnames[] = {"1",
                                                  "2",
                                                  "3",
                                                  "4",
                                                  "5",
                                                  "6",
                                                  "7",
                                                  "8",
                                                  "9",
                                                  "one",
                                                  "two",
                                                  "three",
                                                  "four",
                                                  "five",
                                                  "six",
                                                  "seven",
                                                  "eight",
                                                  "nine"};

    int get_blueprint(const std::string& line) {
        auto first = line.find_first_of("0123456789");
        auto last = line.find_last_of("0123456789");

        return (line[first] - '0') * 10 + (line[last] - '0');
    }

    int get_blueprint2(const std::string& line) {
        auto find_first_named_digit = [&line](int index) {
            long pos = long(line.find(digitsnames[index]));
            int number = index >= 9 ? index - 8 : index + 1;
            return std::pair(number, pos);
        };
        auto find_last_named_digit = [&line](int index) {
            long pos = long(line.rfind(digitsnames[index]));
            int number = index >= 9 ? index - 8 : index + 1;
            return std::pair(number, pos);
        };
        auto found_named_digit = [](std::pair<int, long> pos) {
            return pos.second != long(std::string::npos);
        };

        auto mins = stdv::iota(0, 18) | stdv::transform(find_first_named_digit) | stdv::filter(found_named_digit);
        auto maxs = stdv::iota(0, 18) | stdv::transform(find_last_named_digit) | stdv::filter(found_named_digit);

        auto firstname = stdr::min(mins, std::less<>(), &std::pair<int, long>::second);
        auto lastname = stdr::max(maxs, std::less<>(), &std::pair<int, long>::second);

        int result = 10 * firstname.first + lastname.first;

        return result;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto strings = get_stream<ox::line>(filename);
        auto blueprints = strings | stdv::transform(get_blueprint);
        auto sum = std::accumulate(blueprints.begin(), blueprints.end(), 0);
        printf("%d\n", sum);
        return sum;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto strings = get_stream<ox::line>(filename);
        auto blueprints = strings | stdv::transform(get_blueprint2);
        auto sum = std::accumulate(blueprints.begin(), blueprints.end(), 0);
        printf("%d\n", sum);
        return sum;
    }
} // namespace aoc2023::day01