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

int main(int argc, const char** argv) {
    const char* filename = nullptr;
    bool do_test = false;

    int day = -1, year = -1;

    for (int i = 1; i < argc; ++i) {
        char* end;
        int argument = static_cast<int>(strtol(argv[i], &end, 10));
        if (*end != 0) {
            if (!strcmp(argv[i], "test")) {
                do_test = true;
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

    filename = filename ?: "input";
    if (day < 0) {
        printf("ERROR: No day given");
        exit(1);
    }

    printf("Year %02d, Day %02d:\n", year, day);
    puzzles[year - MIN_YEAR][day-1].first({.day = day, .year = year, .filename = filename});
    puzzles[year - MIN_YEAR][day-1].second({.day = day, .year = year, .filename = filename});
    printf("\n");
    return 0;
}
