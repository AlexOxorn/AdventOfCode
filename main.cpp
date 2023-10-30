#include <cstdio>
#include <cstdlib>
#include <puzzles.h>
#include <cstring>
#include "tester.h"

#define MIN_YEAR 2020

long year = -1, day = -1;
bool do_print = true;

auto get_answer_path() {
    char filename[512] = {};
    sprintf(filename, "%s/../puzzles/%ld/answers.txt", ox::executable_folder().c_str(), year);
    return std::string(filename);
}

int main(int argc, const char** argv) {
    const char* filename = nullptr;
    bool do_test = false;
    std::vector<int>::iterator  it;

    for (int i = 1; i < argc; ++i) {
        char* end;
        long argument = strtol(argv[i], &end, 10);
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

    year = year < 0 ? puzzles.size() - 1 + MIN_YEAR : year;

    if (do_test) {
        std::string answer_path = get_answer_path();

        test(puzzles[year - MIN_YEAR], answer_path);
        return 0;
    }

    filename = filename ?: "input";
    if (day < 0) {
        printf("ERROR: No day given");
        exit(1);
    }

    printf("Year %02ld, Day %02ld:\n", year, day);
    puzzles[year - MIN_YEAR][day-1].first(filename);
    puzzles[year - MIN_YEAR][day-1].second(filename);
    printf("\n");
    return 0;
}
