#include "../../../common.h"
#include <stack>
#include <numeric>
#include <vector>
#include <typeinfo>
#include <algorithm>

#define DAY 10

namespace aoc2021::day10 {
    class parser {
        std::stack<char> state;
        char fail = 0;
    public:
        parser(const std::string& s) {
            for(char c : s) {
                if (is_open(c)) {
                    state.push(c);
                } else {
                    if (!state.empty() && match_close(state.top(), c))
                        state.pop();
                    else {
                        fail = c;
                        break;
                    }
                }
            }
        }

        [[nodiscard]] char get_fail() const {
            return fail;
        }

        static bool is_close(char c) {
            return (c == ')') || (c == ']') || (c == '>') || (c == '}');
        }
        static bool is_open(char c) {
            return (c == '(') || (c == '[') || (c == '<') || (c == '{');
        }
        static bool match_close(char open, char close) {
            switch(open) {
                case '(': return close == ')';
                case '{': return close == '}';
                case '[': return close == ']';
                case '<': return close == '>';
                default: return false;
            }
        }
        static int close_to_score1(char c) {
            switch(c) {
                case ')': return 3;
                case ']': return 57;
                case '}': return 1197;
                case '>': return 25137;
                default: return 0;
            }
        }
        static int open_to_score2(char c) {
            switch(c) {
                case '(': return 1;
                case '[': return 2;
                case '{': return 3;
                case '<': return 4;
                default: return 0;
            }
        }

        [[nodiscard]] long get_completion_score() const {
            std::stack temp{state};
            long sum = 0;
            while (!temp.empty()) {
                sum *= 5;
                sum += open_to_score2(temp.top());
                temp.pop();
            }
            return sum;
        }
    };

    answertype puzzle1(puzzle_options filename) {
        auto input = get_stream<std::string>(filename);

        auto scores = input
                      | stdv::transform([](const parser& p) { return p.get_fail(); })
                      | stdv::filter(std::identity())
                      | stdv::transform(parser::close_to_score1);
        auto x = std::accumulate(scores.begin(), scores.end(), 0);
        myprintf("The score is %d\n", x);
        return x;
    }

    answertype puzzle2(puzzle_options filename) {
        auto input = get_stream<std::string>(filename);

        auto scores_itr = input
                      | stdv::filter([](const parser& p) { return !p.get_fail(); })
                      | stdv::transform([](const parser& p) { return p.get_completion_score(); });

        // For some reason, vector scores(scores_itr.being(), scores_itr.end()) does not work
        // Also gcc error message suck, but also clang 13.0.0 doesn't have full ranges support :(
        std::vector<long> scores/*(scores_itr.begin(), scores_itr.end())*/;
        for (long x : scores_itr) {
            scores.push_back(x);
        }
        std::nth_element(scores.begin(), scores.begin() + (scores.size()/2), scores.end());
        myprintf("The score is %ld\n", scores[scores.size()/2]);
        return scores[scores.size()/2];
    }
}