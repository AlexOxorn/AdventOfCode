#include "../../../common.h"
#include <numeric>
#include <algorithm>
#include <unordered_set>

namespace aoc2023::day05 {
    using lset = std::unordered_set<long>;

    struct seeds {
        std::vector<long> data;
    };

    struct seeds2 {
        std::vector<std::pair<long, long>> data;
        bool in_range(long seed) {
            return stdr::any_of(data, [seed](auto x) {
                auto [start, len] = x;
                long dist = seed - start;
                return (dist >= 0 && dist < len);
            });
        }
    };

    struct int_map {
        long dest_start;
        long source_start;
        long distance;

        [[nodiscard]] bool in_range(long seed) const {
            long dist = seed - source_start;
            return (dist >= 0 && dist < distance);
        }

        [[nodiscard]] long convert(long seed) const {
            long dist = seed - source_start;
            return dest_start + dist;
        }

        [[nodiscard]] bool in_range_rev(long location) const {
            long dist = location - dest_start;
            return (dist >= 0 && dist < distance);
        }

        [[nodiscard]] long convert_rev(long location) const {
            long dist = location - dest_start;
            return source_start + dist;
        }
    };

    struct int_maps {
        std::vector<int_map> maps;

        [[nodiscard]] long convert(long seed) const {
            for (auto& map : maps) {
                if (map.in_range(seed))
                    return map.convert(seed);
            }
            return seed;
        }

        void convert_rev(lset& vec) const {
            for (auto& map : maps) {
                vec.insert(map.dest_start);
            }
            std::vector<long> marks(vec.begin(), vec.end());
            for (long mark : marks) {
                bool can_map_to_self = true;
                for (auto& map : maps) {
                    if (map.in_range_rev(mark))
                        vec.insert(map.convert_rev(mark));
                    if (map.in_range(mark))
                        can_map_to_self = false;
                }
                if (!can_map_to_self) {
                    vec.erase(mark);
                }
            }
        }
    };

    struct pipeline {
        std::vector<int_maps> pipe;
        [[nodiscard]] long run_trough_pipeline(long seed) const {
            for (auto& map : pipe) {
                seed = map.convert(seed);
            }
            return seed;
        }
        [[nodiscard]] lset run_trough_pipeline_rev() const {
            lset vec{};
            for (auto& map : stdv::reverse(pipe)) {
                map.convert_rev(vec);
            }
            return vec;
        }
    };

    std::istream& operator>>(std::istream& in, seeds& s) {
        s.data.clear();
        std::string line;
        std::getline(in, line);
        std::string header;
        std::stringstream ss(line);
        ss >> header;
        long l;
        while (ss >> l) {
            s.data.push_back(l);
        }
        return in;
    }
    std::istream& operator>>(std::istream& in, seeds2& s) {
        s.data.clear();
        std::string line;
        std::getline(in, line);
        std::string header;
        std::stringstream ss(line);
        ss >> header;
        long start, length;
        while (ss >> start >> length) {
            s.data.emplace_back(start, length);
        }
        return in;
    }
    std::istream& operator>>(std::istream& in, int_map& s) {
        return in >> s.dest_start >> s.source_start >> s.distance;
    }
    std::istream& operator>>(std::istream& in, int_maps& s) {
        s.maps.clear();
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty())
                return in;
            std::stringstream ss(line);
            s.maps.emplace_back();
            ss >> s.maps.back();
        }
        return in;
    }
    std::istream& operator>>(std::istream& in, pipeline& s) {
        s.pipe.clear();
        std::string line;
        while (std::getline(in, line)) {
            if (line.empty())
                if (s.pipe.empty() || s.pipe.back().maps.empty())
                    continue;
            s.pipe.emplace_back();
            in >> s.pipe.back();
        }
        return in;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        seeds s;
        pipeline p;
        auto x = get_stream(filename);
        x >> s >> p;
        long min_location = stdr::min(s.data | stdv::transform([&p](long s) { return p.run_trough_pipeline(s); }));
        myprintf("%ld\n", min_location);
        return min_location;
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        seeds2 s;
        pipeline p;
        auto x = get_stream(filename);
        x >> s >> p;
        auto boundaries = p.run_trough_pipeline_rev();
        for (auto [seed, len] : s.data) {
            boundaries.insert(seed);
        }
        long min_location = stdr::min(boundaries | stdv::filter([&s](long l) { return s.in_range(l); })
                                      | stdv::transform([&p](long s) { return p.run_trough_pipeline(s); }));
        myprintf("%ld\n", min_location);
        return min_location;
    }
} // namespace aoc2023::day05