#include "../../../common.h"
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <numeric>

namespace aoc2023::day07 {
    enum HAND_TYPES { DEFAULT, HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_KIND, FULL_HOUSE, FOUR_KIND, FIVE_KIND };

#define JOKER     1
#define JACK      11
#define MAX       15
#define HAND_SIZE 5

    using card = long;
    struct hand : public std::array<card, HAND_SIZE> {
        using std::array<card, HAND_SIZE>::array;
        HAND_TYPES type = DEFAULT;

        static HAND_TYPES determine_type(std::array<int, MAX> counts) {
            int has[4] = {};
            for (auto count : counts) {
                if (count >= 2)
                    ++has[count - 2];
            }
            if (has[3])
                return FIVE_KIND;
            if (has[2])
                return FOUR_KIND;
            if (has[1] && has[0])
                return FULL_HOUSE;
            if (has[1])
                return THREE_KIND;
            if (has[0] == 2)
                return TWO_PAIR;
            if (has[0] == 1)
                return ONE_PAIR;
            return HIGH_CARD;
        }

        HAND_TYPES get_type_recursive_imp(std::array<int, MAX> curr_count = {}, int curr_index = 0) const {
            if (curr_index >= HAND_SIZE) {
                return determine_type(curr_count);
            }

            card c = (*this)[curr_index];
            if (c != JOKER) {
                ++curr_count[c];
                return get_type_recursive_imp(curr_count, curr_index + 1);
            }

            HAND_TYPES max = DEFAULT;
            for (card c = 0; c < MAX; ++c) {
                ++curr_count[c];
                max = std::max(max, get_type_recursive_imp(curr_count, curr_index + 1));
                --curr_count[c];
            }
            return max;
        }

        void set_types() {
            std::array<int, MAX> counts{};
            for (card c : *this) {
                ++counts[c];
            }
            type = determine_type(counts);
        }

        void set_joker_types() { type = get_type_recursive_imp(); }

        bool operator<(const hand& other) const {
            if (type == other.type)
                return stdr::lexicographical_compare(*this, other);
            return type < other.type;
        }
    };

    struct bid : public std::pair<hand, long> {
        static card from_char(char c) {
            switch (c) {
                case 'A': return 14;
                case 'K': return 13;
                case 'Q': return 12;
                case 'J': return JACK;
                case 'T': return 10;
                default: return c - '0';
            }
        }
    };
    struct bid2 : public bid {
        static card from_char(char c) {
            switch (c) {
                case 'A': return 14;
                case 'K': return 13;
                case 'Q': return 12;
                case 'J': return JOKER;
                case 'T': return 10;
                default: return c - '0';
            }
        }
    };

    template <typename Bid> requires (std::is_same_v<Bid, bid> || std::is_same_v<Bid, bid2>)
    std::istream& operator>>(std::istream& in, Bid& b) {
        char card_r[HAND_SIZE];
        for (char& i : card_r)
            in >> i;
        stdr::transform(card_r, b.first.begin(), &Bid::from_char);
        return in >> b.second;
    }

    template <typename Bid, bool Joker>
    answertype solve(puzzle_options filename) {
        auto bids = get_from_input<Bid>(filename);

        auto set_type_func = Joker ? &hand::set_joker_types : &hand::set_types;
        stdr::for_each(bids, set_type_func, &bid::first);

        stdr::sort(bids, std::less<>(), &bid::first);

        auto ordered_bids = bids | stdv::transform(&bid::second);
        std::vector<long> winnings(bids.size());
        stdr::transform(ordered_bids, stdv::iota(1l), winnings.begin(), std::multiplies<>());

        long total_winnings = std::accumulate(winnings.begin(), winnings.end(), 0l);
        myprintf("%ld\n", total_winnings);
        return total_winnings;
    }

    answertype puzzle1([[maybe_unused]] puzzle_options filename) {
        return solve<bid, false>(filename);
    }

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {
        return solve<bid2, true>(filename);
    }
} // namespace aoc2023::day07