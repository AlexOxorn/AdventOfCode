//
// Created by alexoxorn on 2021-12-01.
//

#include "day01.h"
#include <cstdio>
#include <numeric>
#include <functional>
#include <utility>
#include <optional>

#define YEAR 2021
#define DAY 01

namespace day01 {
    DEFINE_DEFAULT_GET_VECTORS(int)

    template<typename T>
    class predicateCounter {
        int& count;
        std::function<bool(T)> predicate;
    public:
        predicateCounter(std::function<bool(T)> pred, int& counter) : predicate(std::move(pred)), count(counter) {}

        predicateCounter& operator=(T t) {
            count += predicate(t);
            return *this;
        }

        predicateCounter& operator*() { return *this; }
        predicateCounter& operator++() { return *this;}
        predicateCounter operator++(int) { return *this; };
    };

    template<std::random_access_iterator InputIt, typename OutputIt>
    OutputIt offset_difference(InputIt first, InputIt last, OutputIt d_first, int offset)
    {
        if (last - first < offset) return d_first;

        for(first += offset; first != last; ++first, ++d_first)
            *d_first = *first - first[-offset];
        return d_first;
    }

    template<std::random_access_iterator I>
    int count_difference(I begin, I end, int offset) {
        int count = 0;
        predicateCounter<int> counter([](int i) {return i>0;}, count);
        offset_difference(begin,end,counter, offset);
        return count;
    }

    template<std::input_iterator I>
    void print_adjacent_increase_count(I begin, I end, int count) {
        std::printf("%d number of increases\n", count_difference(begin,end,count));
    }

    void puzzle1() {
        auto input_vector = get_input();
        print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 1);
    }

    void puzzle2() {
        auto input_vector = get_input();
        print_adjacent_increase_count(input_vector.begin(), input_vector.end(), 3);
    }
}