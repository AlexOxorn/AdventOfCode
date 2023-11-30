#include "tester.h"
#include "common.h"

#include <cstdio>
#include <fstream>
#include <string>

#include <ox/formatting.h>

enum Result { CORRECT = 1, INCORRECT = 0, UNTESTABLE = -1 };

void test(int year, const yearfunctions& functions, const std::string& answer_key_path) {
    ox::format yellow{ox::escape::yellow};
    ox::format red{ox::escape::red};
    ox::format green{ox::escape::green};
    ox::format reset{ox::escape::reset};

    std::ifstream answer_key_file(answer_key_path);
    std::string answer;
    bool skip_test = false;

    if (!answer_key_file) {
        return;
    }

    for (int day = 1; day <= 25; day++) {

        for (int part = 0; part < 2; part++) {
            do_print = false;
            skip_test = false;
            while (true) {
                std::getline(answer_key_file, answer);
                if (answer.empty())
                    continue;
                if (answer == "%PRINT%") {
                    do_print = true;
                    continue;
                }
                if (answer == "%SKIP%") {
                    skip_test = true;
                    continue;
                }
                if (answer == "%END%") {
                    return;
                }
                break;
            }

            printf("%sDAY %2d-%c ", reset.c_str(), day, part == 0 ? 'a' : 'b');
            if (do_print)
                putc('\n', stdout);
            fflush(stdout);
            puzzle_sig puzzle = part == 0 ? functions[day - 1].first : functions[day - 1].second;
            auto result = skip_test ? std::monostate{} : puzzle({.day = day, .year = year, .display = false});

            std::string s = std::visit(answer_to_string{}, result);
            Result res = s == "(nil)" ? UNTESTABLE : static_cast<Result>(s == answer);
            switch (res) {
                case CORRECT: printf("%sPASSED\n", green.c_str()); break;
                case INCORRECT:
                    printf("%sFAILED, our answer was %s, the correct answer is %s\n",
                           red.c_str(),
                           s.c_str(),
                           answer.c_str());
                    break;
                case UNTESTABLE: printf("%sSkipped, answer is %s\n", yellow.c_str(), answer.c_str()); break;
            }
        }
    }
}
