#include "../../../common.h"
#include "ox/ranges.h"
#include <ranges>
#include "ox/debug.h"
#include <limits>
#include <thread>

#define DAY 21

namespace aoc2021::day21 {
    struct part1_simulation {
        constexpr static int winning_score = 1000;
        constexpr static int sides_of_die = 100;
        constexpr static int rolls_per_turn = 3;
        constexpr static int spaces_on_board = 10;
        decltype(stdv::iota(1, sides_of_die+1) | ox::ranges::views::repeat()) deterministic_dice = stdv::iota(1, 101) | ox::ranges::views::repeat();
        decltype(deterministic_dice.begin()) dice_roller = deterministic_dice.begin();
        using player = std::pair<int, int>;
        std::array<player, 2> player_scores;
        int total_number_of_dice_roles = 0;

        part1_simulation(stdr::range auto& f) {
            int starting = 0;
            int player = 0;
            for (const auto& line : f) {
                sscanf(line.c_str(), "Player %d starting position: %d", &player, &starting);
                player_scores[player - 1] = std::make_pair(starting - 1, 0);
            }
        }

        int get_next_roll() {
            total_number_of_dice_roles++;
            return *dice_roller++;
        }

        bool take_turn(int player) {
            auto& [player_placement, player_score] = player_scores.at(player);

            for ([[maybe_unused]] int i : stdv::iota(0, rolls_per_turn)) {
                int move = get_next_roll();
                player_placement += move;
            }
            player_placement %= spaces_on_board;
            player_score += player_placement + 1;
            return player_score >= winning_score;
        }

        int simulate() {
            int starting_player = 0;
            for (; !take_turn(starting_player); starting_player = !starting_player)
                ;
            myprintf("The winning player is %d with score %d\n",
                   starting_player + 1, player_scores[starting_player].second);
            myprintf("Puzzle solution is %d x %d = %d\n",
                   total_number_of_dice_roles, player_scores[!starting_player].second,
                   total_number_of_dice_roles * player_scores[!starting_player].second);
            return total_number_of_dice_roles * player_scores[!starting_player].second;
        }
    };


    template <int Side, int Rolls>
    constexpr std::array<std::pair<int, int>, Side * Rolls> get_die_frequency() {
        if constexpr (Rolls == 1) {
            std::array<std::pair<int, int>, Side * Rolls> to_return{};
            auto in = stdv::iota(1, Side+1) | stdv::transform([](int i) { return std::make_pair(i, 1); });
            std::copy(in.begin(), in.end(), to_return.begin());
            return to_return;
        } else {
            std::array<std::pair<int, int>, Side * Rolls> to_return{};
            auto prev = get_die_frequency<Side, Rolls - 1>();
            for (int i : stdv::iota(1, Side+1)) {
                for(auto prev_val : prev) {
                    int val = i + prev_val.first;
                    int new_freq = prev_val.second + to_return[val-1].second;
                    to_return[val-1] = {val, new_freq};
                }
            }
            return to_return;
        }
    }

    template <int Side, int Rolls>
    constexpr static std::array<std::pair<int, int>, Side * Rolls> die_frequency = get_die_frequency<Side, Rolls>();

    struct part2_simulation {
        constexpr static int winning_score = 21;
        constexpr static int sides_of_die = 3;
        constexpr static int rolls_per_turn = 3;
        constexpr static int spaces_on_board = 10;
        using player = std::pair<int, int>;
        std::array<int, 2> starting_positions{};
        std::array<std::atomic_long, 2> number_of_victories{};

        part2_simulation(stdr::range auto& f) {
            int starting = 0;
            int player = 0;
            for (const auto& line : f) {
                sscanf(line.c_str(), "Player %d starting position: %d", &player, &starting);
                starting_positions[player - 1] = starting - 1;
            }
        }

        void _recursion(const std::array<player, 2>& players, int current_player, long identical_universes = 1) {
            for (auto quantum : die_frequency<sides_of_die, rolls_per_turn>) {
                if (quantum.second == 0)
                    continue;
                auto cpy = players;
                auto& [position, score] = cpy[current_player];
                position += quantum.first;
                position %= spaces_on_board;
                score += (position + 1);
                if (score >= winning_score) {
                    number_of_victories[current_player] += (identical_universes * quantum.second);
                    continue;
                }
                _recursion(cpy, !current_player, identical_universes * quantum.second);
           }
        }

        long simulate() {
#ifdef _LIBCPP_VERSION
            volatile std::atomic_bool stop = false;
            std::thread a([this, &stop]() {
#else
            std::jthread a([this](const std::stop_token& stop_token) {
#endif
                using namespace std::literals::chrono_literals;
#ifdef _LIBCPP_VERSION
                while (!stop.load()) {
#else
                while(!stop_token.stop_requested()) {
#endif
                    myprintf("%ld | %ld\r", number_of_victories[0].load(), number_of_victories[1].load());
                    fflush(stdout);
                    std::this_thread::sleep_for(100ms);
                }
            });

            _recursion({{{starting_positions[0], 0}, {starting_positions[1], 0}}}, 0);
#ifdef _LIBCPP_VERSION
            stop = true;
#else
            a.request_stop();
#endif
            myprintf("player 1 won %ld times\n", number_of_victories[0].load());
            myprintf("player 2 won %ld times\n", number_of_victories[1].load());
            return std::max(number_of_victories[0].load(), number_of_victories[1].load());
#ifdef _LIBCPP_VERSION
            a.join();
#endif
        }
    };




    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<ox::line>(filename);
        part1_simulation s(input);
        return s.simulate();
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<ox::line>(filename);
        part2_simulation s(input);
        return s.simulate();
    }
} // namespace aoc2021::day21
