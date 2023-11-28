import sys
import os.path

base = """#include "../../../common.h"

namespace aoc{year}::day{day:02} {{
    answertype puzzle1([[maybe_unused]] puzzle_options filename) {{
        return {{}};
    }}

    answertype puzzle2([[maybe_unused]] puzzle_options filename) {{
        return {{}};
    }}
}} // namespace aoc{year}::day{day:02}"""


def main():
    year = int(input("year: "))
    for day in range(1, 26):
        filepath = f"./puzzles/{year}/src/day{day:02}.cpp"
        if os.path.isfile(filepath):
            continue
        with open(filepath, "w") as f:
            f.write(base.format(year=year, day=day))


if __name__ == "__main__":
    main()
