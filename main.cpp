#include <cstdio>
#include <cstdlib>
#include <puzzles.h>

#define MIN_YEAR 2021

long year = -1, day = -1;

int main(int argc, const char** argv) {
    const char* filename = nullptr;

    for (int i = 1; i < argc; ++i) {
        char* end;
        long argument = strtol(argv[i], &end, 10);
        if (*end != 0) {
            filename = argv[i];
            continue;
        }
        (argument < 1000 ? day : year) = argument;
    }

    year = year < 0 ? puzzles.size() - 1 + MIN_YEAR : year;
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
