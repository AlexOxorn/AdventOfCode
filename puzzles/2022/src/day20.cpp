#include "../../../common.h"
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <queue>
#include <numeric>
#include <functional>
#include <cassert>

namespace aoc2022::day20 {
    long sign(long i) {
        if (i == 0)
            return 0;
        return std::abs(i) / i;
    }

    long clamp(long diff, long base, long max) {
        if (diff == 0)
            return base;
        if (diff < 0) {
            long temp = (diff + base - 1) % (max - 1);
            if (temp < 0)
                temp += (max - 1);
            return temp + 1;
        }
        return (diff + base - 1) % (max - 1) + 1;
    }

    void print_message(std::vector<long>& encyption) {
        for (auto x : encyption) {
            myprintf("%12ld ", x);
        }
        myprintf("\n");
    }

    void print_message2(std::vector<long>& encyption, std::vector<long>& indecies) {
        for (long index : indecies) {
            myprintf("%12ld ", encyption[index]);
        }
        myprintf("\n");
    }

    auto solve(puzzle_options filename, int repeat = 1, long decryption_key = 1) {
        auto encrypted = get_from_input<long>(filename);
        stdr::transform(encrypted, encrypted.begin(), std::bind_front(std::multiplies<>(), decryption_key));
        long message_size = static_cast<long>(encrypted.size());
        std::vector<long> decryption_index(encrypted.size());
        std::iota(decryption_index.begin(), decryption_index.end(), 0);

        for (int iteration = 0; iteration < repeat; ++iteration) {
            for (long i = 0; i < message_size; ++i) {
                if (message_size <= 10) {
                    myprintf("Moving %-12ld: ", encrypted[decryption_index[i]]);
                    print_message(encrypted);
                }
                long index = decryption_index[i];
                long val = encrypted[index];
                long clamped = clamp(val, index, message_size);
                auto [start, end] = std::minmax(index, clamped);
                for (auto& x : decryption_index) {
                    if (start <= x && x <= end) {
                        x -= sign(clamped - index);
                    }
                }

                decryption_index[i] = clamped;
                encrypted.erase(encrypted.begin() + index);
                encrypted.insert(encrypted.begin() + clamped, val);
            }
            if (message_size <= 10) {
                myprintf("Final Result       : ");
                print_message(encrypted);
                myprintf("====================================\n");
            }
        }

        long sum = 0;
        long zero_index = stdr::find(encrypted, 0) - encrypted.begin();
        sum += encrypted[(zero_index + 1000) % message_size];
        sum += encrypted[(zero_index + 2000) % message_size];
        sum += encrypted[(zero_index + 3000) % message_size];
        myprintf("The 3 sums post decryption %ld\n", sum);
        return sum;
    }

    answertype puzzle1(puzzle_options filename) {
        return solve(filename);
    }

    answertype puzzle2(puzzle_options filename) {
        return solve(filename, 10, 811589153);
    }
} // namespace aoc2022::day20