#include "../../../common.h"
#include <array>
#include <sstream>
#include <algorithm>
#include <ranges>
#include <numeric>
#include "ox/grid.h"

#define DAY 04

namespace aoc2021::day04 {
    constexpr int bingosize = 5;
    constexpr int poolsize = 99;
    using ball_element = std::pair<int, bool> *;
    using bingo_array = std::array<ball_element, bingosize * bingosize>;
    using bingo_inputs = std::vector<int>;

    static struct number_pool {
        using numberstate = std::pair<int, bool>;
        std::array<numberstate, poolsize + 1> pool;

        number_pool() {
            for (int i = 0; i <= poolsize; i++) {
                pool[i].first = i;
            }
        }

        void reset() {
            for (auto& n: pool) {
                n.second = false;
            }
        }
    } main_pool;

    struct bingo_card : public ox::grid<ball_element, bingo_array> {
        [[nodiscard]] bool check_vertical_for_win() const {
            return stdr::any_of(
                    stdv::iota(0, bingosize),
                    [this](int i) { return stdr::all_of(
                            stdv::iota(0, bingosize),
                            [this, i](int j) { return at(i, j)->second; }
                    );
            });
        }

        [[nodiscard]] bool check_horizontal_for_win() const {
            return std::any_of(begin(), end(), [](auto subrange) {
                return std::all_of(subrange.begin(), subrange.end(), [](auto &element) { return element->second; });
            });
        }

        [[nodiscard]] bool check_diagonal_for_win() const {
            return stdr::all_of(stdv::iota(0, bingosize), [this](int i) { return at(i, i)->second; }) ||
                   stdr::all_of(stdv::iota(0, bingosize), [this](int i) { return at(i, 4 - i)->second; });
        }

        [[nodiscard]] bool check_for_win() const {
            return check_horizontal_for_win() || check_vertical_for_win() /*|| check_diagonal_for_win()*/;
        }

        [[nodiscard]] int get_score() const {
            auto unmarked = data
                            | stdv::filter([](ball_element b) { return !b->second; })
                            | stdv::transform([](ball_element b) { return b->first; });
            return std::accumulate(unmarked.begin(), unmarked.end(), 0);
        }

        void print_state() {
            leveled_foreach(
                    [](auto& elem) { myprintf("\033[%dm%2d\033[0m ", elem->second ? 31 : 0, elem->first); },
                    []() { myprintf("\n"); });
            myprintf("\n");
        }
    };

    std::istream &operator>>(std::istream &in, bingo_card &bc) {
        bc.set_width(bingosize);
        for (int i: stdv::iota(0, bingosize)) {
            for (int j: stdv::iota(0, bingosize)) {
                int x;
                if (!(in >> x)) {
                    return in;
                }
                bc.at(i, j) = &main_pool.pool.at(x);
            }
        }
        return in;
    }

    bingo_inputs extract_roller(ox::ifstream_container<bingo_card> &in) {
        bingo_inputs to_return;
        std::string bingo_inputs;
        std::getline(in, bingo_inputs);
        std::stringstream ss(bingo_inputs);
        std::string number;
        while (getline(ss, number, ',')) {
            to_return.push_back(std::stoi(number));
        }
        in.ignore(std::numeric_limits<unsigned>::max(), '\n');
        return to_return;
    }

    int print_final_result(bingo_card card, int ball) {
        int score = card.get_score();
        myprintf("Total Score is: %d\n", card.get_score());
        myprintf("Last called was: %d\n", ball);
        myprintf("Their product was: %d\n", ball * score);
        return ball * score;
    }

    std::pair<bingo_inputs, std::vector<bingo_card>> init(puzzle_options filename) {
        main_pool.reset();
        auto input = get_stream<bingo_card>(filename);
        return {extract_roller(input), {input.begin(), input.end()}};
    }

    answertype puzzle1(puzzle_options filename) {
        auto[roller, cards] = init(filename);
        for (auto ball: roller) {
            myprintf("####################\nCurrent Ball: %d\n####################\n", ball);
            main_pool.pool.at(ball).second = true;
            for (auto card: cards) {
                card.print_state();
                if (card.check_for_win()) {
                    return print_final_result(card, ball);
                }
            }
        }
        __builtin_unreachable();
    }

    answertype puzzle2(puzzle_options filename) {
        auto[roller, cards] = init(filename);
        for (auto ball: roller) {
            myprintf("####################\nCurrent Ball: %d\n####################\n", ball);
            main_pool.pool.at(ball).second = true;
            auto endpoint = std::remove_if(cards.begin(), cards.end(), [](auto card) {
                card.print_state();
                return card.check_for_win();
            });
            if (endpoint == cards.begin()) {
                return print_final_result(cards.front(), ball);
            }
            cards.erase(endpoint, cards.end());
        }
        __builtin_unreachable();
    }
}
