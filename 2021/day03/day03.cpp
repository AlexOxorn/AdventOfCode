//
// Created by alexoxorn on 2021-12-03.
//

#include "day03.h"
#include <array>
#include <sstream>
#include <numeric>

#define YEAR 2021
#define DAY 03

namespace day03{
    constexpr int power_of_2(unsigned x) {
        if (x == 0) return 1;
        int to_return = 1;
        for(int i = 0; i < x; i++) {
            to_return <<= 1;
        }
        return to_return;
    }

    class gamma_rate_part {
        std::vector<int> part{};
        int total_count = 0;

        void set_array(const std::string& s) {
            part.clear();
            for (char c : s) {
                part.push_back(c - '0');
            }
            total_count = 1;
        }
    public:
        [[nodiscard]] unsigned get_most_common() const {
            int half = (total_count+1)/2;
            int base = 1;
            unsigned result = 0;
            for(auto itr = part.rbegin(); itr != part.rend(); itr++, base <<= 1) {
                if (*itr >= half)
                    result += base;
            }
            return result;
        }
        std::pair<int, int> gamma_epsilon_rate() {
            auto result = get_most_common();
            return {result, (~result) % (1 << part.size())};
        }

        gamma_rate_part& operator+(const gamma_rate_part &other) {
            size_t size = std::max(part.size(), other.part.size());
            part.resize(size);
            for(int i = 0; i < part.size(); i++) {
                part[i] += other.part[i];
            }
            total_count += other.total_count;
            return *this;
        }
        friend std::istream& operator>>(std::istream& in, gamma_rate_part& g);
        friend std::vector<int>& operator+(std::vector<int>&&, const gamma_rate_part& g);
    };

    std::istream& operator>>(std::istream& in, gamma_rate_part& g) {
        std::string s;
        std::getline(in, s);
        g.set_array(s);
        return in;
    }

    std::vector<int>& operator+(std::vector<int>&& total, const gamma_rate_part &g) {
        total.resize(power_of_2(g.part.size()));
        total.at(g.get_most_common()) += 1;
        return total;
    }

    template <std::random_access_iterator iter, typename Compare = std::greater<int>>
    int recursive_decent(const iter true_begin, iter end, Compare c = Compare()) {
        std::random_access_iterator auto begin = true_begin;
        while((begin + 1) != end) {
            iter mid = begin + std::distance(begin, end) / 2;
            auto sum1 = std::accumulate(begin, mid, 0);
            auto sum2 = std::accumulate(mid, end, 0);
            if (sum1 + sum2 == 1) {
                return std::find(begin, end, 1) - true_begin;
            }
            if (c(sum1, sum2))
                end = mid;
            else
                begin = mid;
        }
        return begin - true_begin;
    }

    void puzzle1() {
        auto input_stream = GET_STREAM(input, gamma_rate_part);
        auto final = std::accumulate(input_stream.begin(), input_stream.end(), gamma_rate_part());
        auto[gamma, epsilon] = final.gamma_epsilon_rate();
        printf("gamma is %d and epsilon is %d\n", gamma, epsilon);
        printf("their product is: %d\n", gamma * epsilon);
    }

    void puzzle2() {
        auto input_stream = GET_STREAM(input, gamma_rate_part);
        auto binary_tree = std::accumulate(input_stream.begin(), input_stream.end(), std::vector<int>());
        int o2 = recursive_decent(binary_tree.begin(), binary_tree.end());
        int co2 = recursive_decent(binary_tree.begin(), binary_tree.end(), std::less_equal<>());
        printf("O2  is %d\n", o2);
        printf("CO2 is %d\n", co2);
        printf("Their product is %d\n", co2 * o2);
    }
}
