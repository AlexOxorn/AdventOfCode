#include "../../../common.h"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace aoc2023::day06 {
    using times = std::vector<long>;

    std::pair<times, times> parse_data(std::istream& in) {
        std::pair<times, times> to_return;
        std::string dump;
        in >> dump;
        long time;
        while (in >> time) {
            to_return.first.push_back(time);
        }
        in.clear();
        in >> dump;
        while (in >> time) {
            to_return.second.push_back(time);
        }
        return to_return;
    }

    bool is_number(const std::string& s) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isdigit(*it))
            ++it;
        return !s.empty() && it == s.end();
    }

    std::pair<long, long> parse_data2(std::istream& in) {
        std::pair<long, long> to_return;
        std::string dump;
        std::string temp_num;
        in >> dump;
        while (in >> dump) {
            if (!is_number(dump))
                break;
            temp_num += dump;
        }
        to_return.first = std::atol(temp_num.c_str());
        temp_num.clear();
        while (in >> dump) {
            temp_num += dump;
        }
        to_return.second = std::atol(temp_num.c_str());
        return to_return;
    }

    std::pair<double, double> find_roots(double a, double b, double c) {
        double root = std::sqrt(b * b - 4 * a * c);
        return {(-b + root) / (2 * a), (-b - root) / (2 * a)};
    }

    long get_time_range(long time, long record) {
        auto [lower, upper] = find_roots(-1.0, double(time), double(-record));
        return long(std::floor(std::nextafter(upper, lower)) - std::ceil(std::nextafter(lower, upper))) + 1;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto in_file = get_stream(filename);
        auto [times, records] = parse_data(in_file);
        std::vector<long> ranges(times.size());
        stdr::transform(times, records, ranges.begin(), get_time_range);
        long answer = std::accumulate(ranges.begin(), ranges.end(), 1l, std::multiplies<>());
        myprintf("%ld\n", answer);
        return answer;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        auto in_file = get_stream(filename);
        auto [time, record] = parse_data2(in_file);
        long answer = get_time_range(time, record);
        myprintf("%ld\n", answer);
        return answer;
    }
} // namespace aoc2023::day06