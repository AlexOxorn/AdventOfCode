#include "../../../common.h"
#include <algorithm>
#include <deque>
#include <stack>
#include <functional>

namespace aoc2022::day05 {
    unsigned line_index_to_stack(unsigned i) {
        return (i - 1) / 4;
    }

    struct direction {
        int amount;
        int start;
        int end;
        friend STREAM_IN(direction, dir) {
            std::string line;
            std::getline(in, line);
            sscanf(line.c_str(), "move %d from %d to %d", &dir.amount, &dir.start, &dir.end);
            return in;
        }
    };

    struct crate_stacks : std::array<std::deque<char>, 9> {
        void print_stack() {
            for (auto& a : *this) {
                if (a.empty()) {
                    break;
                }
                for (auto& b : a) {
                    myprintf("%c", b);
                }
                myprintf("\n");
            }
        }

        friend STREAM_IN(crate_stacks, cs) {
            std::string s;
            while (std::getline(in, s), !s.empty()) {
                for (unsigned i = 1; i < s.length(); i += 4) {
                    char rep = s[i];
                    if ('A' <= rep && rep <= 'Z')
                        cs[line_index_to_stack(i)].push_front(rep);
                }
            }

            return in;
        }

        std::deque<char>& at(int stack_number) { return (*this)[stack_number - 1]; }

        void apply_direction9000(direction dir) {
            for (int i = 0; i < dir.amount; i++) {
                auto& end = this->at(dir.end);
                auto& start = this->at(dir.start);
                end.push_back(start.back());
                start.pop_back();
            }
        }

        void apply_direction9001(direction dir) {
            std::stack<char> temp;
            for (int i = 0; i < dir.amount; i++) {
                auto& start = this->at(dir.start);
                temp.push(start.back());
                start.pop_back();
            }
            for (int i = 0; i < dir.amount; i++) {
                auto& end = this->at(dir.end);
                end.push_back(temp.top());
                temp.pop();
            }
        }
    };

    answertype solver(puzzle_options filename, void (crate_stacks::*move)(direction dir)) {
        auto input = get_stream<direction>(filename);
        crate_stacks stacks;
        input >> stacks;
        for (direction x : input) {
            (stacks.*move)(x);
        }
        auto res_stream = stacks | stdv::filter(std::not_fn<>(&std::deque<char>::empty))
                        | stdv::transform((char& (std::deque<char>::*) ()) & std::deque<char>::back);
        std::string result(res_stream.begin(), res_stream.end());
        myprintf("The Top Line is %s\n", result.c_str());
        return result;
    }

    answertype puzzle1(puzzle_options filename) {
        return solver(filename, &crate_stacks::apply_direction9000);
    }

    answertype puzzle2(puzzle_options filename) {
        return solver(filename, &crate_stacks::apply_direction9001);
    }
} // namespace aoc2022::day05