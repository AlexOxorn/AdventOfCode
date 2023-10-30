//
// Created by alexoxorn on 10/29/23.
//

#include "tester.h"
#include "common.h"

#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <fstream>
#include <string>

#include <ox/algorithms.h>
#include <ox/formatting.h>

static const int std_backup = dup(STDOUT_FILENO);
static const int devnull = open("/dev/null", O_WRONLY);

void redirect_stdout(int fileno) {
    fflush(stdout);
    dup2(fileno, STDOUT_FILENO);
}

enum Result { CORRECT = 1, INCORRECT = 0, UNTESTABLE = -1 };

struct answer_comparison {
    Result operator()(std::integral auto i, const std::string& answer) {
        return std::to_string(i) == answer ? CORRECT : INCORRECT;
    }
    Result operator()(const std::string& s, const std::string& answer) { return s == answer ? CORRECT : INCORRECT; }
    Result operator()(const std::monostate&, const std::string&) { return UNTESTABLE; }
};

struct answer_to_string {
    std::string operator()(std::integral auto i) { return std::to_string(i); }
    std::string operator()(const std::string& s) { return s; }
    std::string operator()(const std::monostate&) { return "(nil)"; }
};

void test(const yearfunctions& functions, const std::string& answer_key_path) {
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

    //    std::array<puzzle_sig, 50> tests = ox::flatten(functions);

    for (day = 1; day <= 25; day++) {

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

            printf("%sDAY %2ld-%c ", reset.c_str(), day, part == 0 ? 'a' : 'b');
            if (do_print)
                putc('\n', stdout);
            fflush(stdout);
            puzzle_sig puzzle = part == 0 ? functions[day - 1].first : functions[day - 1].second;
            auto result = skip_test ? std::monostate{} : puzzle("input");

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
