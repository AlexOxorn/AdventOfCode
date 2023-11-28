import os.path
import sys

import aocd.exceptions
from aocd.models import Puzzle
from aocd.get import most_recent_year
from time import sleep


def main():
    print(sys.argv)
    year_str = sys.argv[1] if len(sys.argv) > 1 else input("Year: ")
    year = most_recent_year() if year_str == 'default' else int(year_str)

    try:
        for day in range(1, 26):
            puzzle = Puzzle(day=day, year=year)
            read = False
            # Main Input File
            filepath = f"./puzzles/{year}/inputs/day{day:02}_input.txt"
            if not os.path.isfile(filepath):
                print(f"Reading day Input {day}")
                input_data = puzzle.input_data
                with open(filepath, "w") as f:
                    f.write(input_data)
                read = True

            # Example Files
            if not (os.path.isfile(f"./puzzles/{year}/inputs/day{day:02}_sample_input.txt") or
                    os.path.isfile(f"./puzzles/{year}/inputs/day{day:02}_sample1_input.txt")):
                print(f"Reading day Example {day}")
                example_data = puzzle.examples
                for i, data in enumerate(ex.input_data for ex in example_data):
                    filepath = (f"./puzzles/{year}/inputs/day{day:02}_sample"
                                f"{i+1 if len(example_data) > 1 else ''}_input.txt")
                    if not os.path.isfile(filepath):
                        with open(filepath, "w") as f:
                            f.write(data)
                read = True
            if read:
                print("\tDone")
                sleep(2)
    except aocd.exceptions.PuzzleLockedError:
        pass


if __name__ == "__main__":
    main()
