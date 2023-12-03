#include <cstdio>
#include <cstdlib>
#include <puzzles.h>
#include <cstring>
#include "tester.h"

#include <thread>

#define MIN_YEAR 2020

long year = -1, day = -1;
bool do_print = true;

auto get_answer_path(int year) {
    char filename[512] = {};
    sprintf(filename, "%s/../puzzles/%d/answers.txt", ox::executable_folder().c_str(), year);
    return std::string(filename);
}

static const char base[] =
        R"COMMAND(python -c "import aocd; aocd.submit(%s, part='%c', day=%d, year=%d)" 2>/dev/null)COMMAND";
using unique_process = std::unique_ptr<FILE, decltype([](FILE* f) { pclose(f); })>;

void submit(const answertype& ans, int year, int day, int part) {
    char command[sizeof(base) + 20];
    char read_output[1024];

    auto str_ans = std::visit(answer_to_string2{}, ans);
    if (!str_ans)
        return;
    sprintf(command, base, str_ans->c_str(), 'a' - 1 + part, day, year);
    unique_process f{popen(command, "r")};
    while (fgets(read_output, sizeof read_output, f.get()) != nullptr) {
        std::string_view temp = read_output;
        if (temp.contains("\033[31m")) {
            auto endofsentence = temp.find('.');
            std::string substring{temp.substr(0, endofsentence)};
            printf("%s\n\033[0m", substring.c_str());
            return;
        }
        if (temp.contains("incorrect")) {
            printf("\033[31m%s\n\033[0m", read_output);
            return;
        }
    }
    printf("\033[32mThe answer is correct :)\033[0m\n");
}

int main(int argc, const char** argv) {
    const char* filename = nullptr;
    bool do_test = false;
    bool do_submit = false;

    int day = -1, year = -1;
    int part = 0b11;

    for (int i = 1; i < argc; ++i) {
        char* end;
        int argument = static_cast<int>(strtol(argv[i], &end, 10));
        if (*end != 0) {
            if (!strcmp(argv[i], "test")) {
                do_test = true;
            } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--part")) {
                ++i;
                part = atoi(argv[i]);
            } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--do_submit")) {
                do_submit = true;
            } else {
                filename = argv[i];
            }
            continue;
        }
        (argument < 1000 ? day : year) = argument;
    }

    year = year < 0 ? static_cast<int>(puzzles.size() - 1 + MIN_YEAR) : year;

    if (do_test) {
        do_print = false;
        std::string answer_path = get_answer_path(year);

        test(year, puzzles[year - MIN_YEAR], answer_path);
        return 0;
    }

    filename = filename ?: "";
    if (day < 0) {
        printf("ERROR: No day given");
        exit(1);
    }

    printf("Year %02d, Day %02d:\n", year, day);
    puzzle_options opt{.day = day, .year = year, .filename = filename};
    auto puzzle = puzzles[year - MIN_YEAR][day - 1];
    if (part & 0b01) {
        auto ans = puzzle.first(opt);
        if (do_submit) {
            submit(ans, year, day, 1);
        }
    }
    if (part & 0b10) {
        auto ans = puzzle.second(opt);
        if (do_submit) {
            submit(ans, year, day, 2);
        }
    }
    return 0;
}
