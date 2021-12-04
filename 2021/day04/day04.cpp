//
// Created by alexoxorn on 2021-12-04.
//

#include "day04.h"
#include <array>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <numeric>

#define YEAR 2021
#define DAY 04

namespace day04 {
    constexpr int bingosize = 5;
    constexpr int poolsize = 99;
    using bingo_array = std::array<std::array<std::pair<int, bool>*, bingosize>, bingosize>;
    using bingo_inputs = std::vector<int>;

    static struct number_pool {
        using numberstate = std::pair<int, bool>;
        std::array<numberstate, poolsize+1> pool;

        number_pool() {
            for(int i = 0; i <= poolsize; i++) {
                pool[i].first = i;
            }
        }

        void reset() {
            for(auto n : pool) {
                n.second = false;
            }
        }
    } main_pool;

    struct bingo_card {
        bingo_array array;

        [[nodiscard]] bool check_vertical_for_win() const {
            return std::ranges::any_of(std::views::iota(0, bingosize),
                                [this](int i) {
                                    return std::ranges::all_of(std::views::iota(0, bingosize),
                                                               [this, i](int j) { return array[i][j]->second; });
            });
        }

        [[nodiscard]] bool check_horizontal_for_win() const {
            return std::ranges::any_of(std::views::iota(0, bingosize),
                                       [this](int j) {
                                           return std::ranges::all_of(std::views::iota(0, bingosize),
                                                                      [this, j](int i) { return array[i][j]->second; });
            });
        }

        [[nodiscard]] bool check_diagonal_for_win() const {
            return std::ranges::all_of(std::views::iota(0, bingosize),
                                       [this](int i) { return array[i][i]->second; }) ||
                    std::ranges::all_of(std::views::iota(0, bingosize),
                                        [this](int i) { return array[i][4-i]->second; });
        }
        
        [[nodiscard]] bool check_for_win() const {
            return check_horizontal_for_win() || check_vertical_for_win() /*|| check_diagonal_for_win()*/;
        }

        [[nodiscard]] int get_score() const {
            int score = 0;
            for(int i = 0; i < bingosize; i++) {
                for (int j = 0; j < bingosize; j++){
                    if (!array[i][j]->second)
                        score += array[i][j]->first;
                }
            }
            return score;
        }
        void print_state() {
            for (auto x : array) {
                for (auto y : x) {
                    printf("\033[%dm%2d\033[0m ", y->second ? 31 : 0, y->first);
                }
                printf("\n");
            }
            printf("\n");
        }
    };

    std::istream& operator >>(std::istream& in, bingo_card& bc) {
        for(int i = 0; i < bingosize; i++) {
            for (int j = 0; j < bingosize; j++){
                int x;
                if (!(in >> x)) {
                    return in;
                }
                bc.array[i][j] = &main_pool.pool.at(x);
            }
        }
        return in;
    }

    bingo_inputs extract_roller(ifstream_container<bingo_card>& in) {
        bingo_inputs to_return;
        std::string bingo_inputs;
        std::getline(in, bingo_inputs);
        std::stringstream ss(bingo_inputs);
        std::string number;
        while(getline(ss, number, ',')) {
            to_return.push_back(std::stoi(number));
        }
        in.ignore(std::numeric_limits<unsigned>::max(), '\n');
        return to_return;
    }

    void print_final_result(bingo_card card, int ball) {
        int score = card.get_score();
        printf("Total Score is: %d\n", card.get_score());
        printf("Last called was: %d\n", ball);
        printf("Their product was: %d\n", ball * score);
    }

    std::pair<bingo_inputs, std::vector<bingo_card>> init() {
        main_pool.reset();
        auto input = GET_STREAM(input, bingo_card);
        return {extract_roller(input), {input.begin(), input.end()}};
    }

    void puzzle1() {
        auto [roller, cards] = init();
        for(auto ball : roller) {
            printf("####################\nCurrent Ball: %d\n####################\n", ball);
            main_pool.pool.at(ball).second = true;
            for (auto card : cards) {
                card.print_state();
                if (card.check_for_win()) {
                    print_final_result(card, ball);
                    return;
                }
            }
        }
    }

    void puzzle2() {
        auto [roller, cards] = init();
        for(auto ball : roller) {
            printf("####################\nCurrent Ball: %d\n####################\n", ball);
            main_pool.pool.at(ball).second = true;
            auto endpoint = std::remove_if(cards.begin(), cards.end(), [](auto card) {
                card.print_state();
                return card.check_for_win();
            });
            if(endpoint == cards.begin()) {
                print_final_result(cards.front(), ball);
                return;
            }
            cards.erase(endpoint, cards.end());
        }
    }
}
