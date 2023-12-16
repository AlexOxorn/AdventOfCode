#include "../../../common.h"
#include <algorithm>
#include <unordered_map>
#include <cassert>
#include <limits>
#include <numeric>

namespace aoc2023::day15 {
    enum OP : char { ADD = '=', REMOVE = '-' };

    struct instruction {
        std::string key;
        int value;
        OP op;
    };

    struct csv_string : std::string {
        [[nodiscard]] instruction split() const {
            instruction to_return;
            auto op_pos = this->find_first_of("=-");
            to_return.op = OP((*this)[op_pos]);
            to_return.key = this->substr(0, op_pos);
            if (to_return.op == ADD) {
                to_return.value = atoi(this->c_str() + op_pos + 1);
            } else {
                to_return.value = 0;
            }
            return to_return;
        }
    };

    int HASH(const std::string& s) {
        int res = 0;
        for (char c : s) {
            res += c;
            res *= 17;
            res %= 256;
        }
        return res;
    }

    class HASHMAP {
        using value_type = std::pair<std::string, int>;
        using bucket_type = std::vector<value_type>;
    public:
        std::vector<bucket_type> buckets{256};
        int& operator[](const std::string& key) {
            bucket_type& bucket = buckets[HASH(key)];
            if (auto pos = stdr::find_if(bucket, [&](const value_type& v) { return v.first == key; });
                pos != bucket.end()) {
                return pos->second;
            } else {
                bucket.emplace_back(key, 0);
                return bucket.back().second;
            }
        }

        void erase(const std::string& key) {
            bucket_type& bucket = buckets[HASH(key)];
            if (auto pos = stdr::find_if(bucket, [&](const value_type& v) { return v.first == key; });
                pos != bucket.end()) {
                bucket.erase(pos);
            }
        }
    };

    std::istream& operator>>(std::istream& in, std::string& s) {
        return std::getline(in, s, ',');
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        auto instructions = get_stream<csv_string>(filename);
        auto hashes = instructions | stdv::transform(HASH);
        int res = std::accumulate(hashes.begin(), hashes.end(), 0);
        printf("%d\n", res);
        return res;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        HASHMAP hashmap;

        auto instructions_strings = get_stream<csv_string>(filename);
        auto instructions = instructions_strings | stdv::transform(&csv_string::split);
        for (const auto& [key, value, op] : instructions) {
            switch (op) {
                case ADD: hashmap[key] = value; break;
                case REMOVE: hashmap.erase(key);
            }
        }

#ifdef __cpp_lib_ranges_enumerate
        for (const auto& [i, bucket] : hashmap.buckets | stdv::enumerate) {
            printf("BOX %3ld:\t", i + 1);
            printf("[");
            for (auto [key, value] : bucket)
                printf("(%s, %d), ", key.c_str(), value);
            printf("]\n");
        }

        long res = 0l;
        for (const auto& [bucket_index, bucket] : hashmap.buckets | stdv::enumerate) {
            for (const auto& [lens_index, lens] : bucket | stdv::values | stdv::enumerate) {
                res += (bucket_index + 1) * (lens_index + 1) * lens;
            }
        }
#else
        for (int i = 0; i < 256; ++i) {
            const auto& bucket = hashmap.buckets[i];
            printf("BOX %3d:\t", i + 1);
            printf("[");
            for (auto [key, value] : bucket)
                printf("(%s, %d), ", key.c_str(), value);
            printf("]\n");
        }

        long res = 0l;
        for (int bucket_index = 0; bucket_index < 256; ++bucket_index) {
            const auto& bucket = hashmap.buckets[bucket_index];
            for (int lens_index = 0; lens_index < int(bucket.size()); ++lens_index) {
                const auto& lens = bucket[lens_index].second;
                res += (bucket_index + 1) * (lens_index + 1) * lens;
            }
        }
#endif

        printf("%ld\n", res);
        return res;
    }
} // namespace aoc2023::day15