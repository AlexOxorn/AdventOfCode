#include <array>
#include "../include/2020.h"

namespace aoc2020 {
    PER_DAY(INCLUDE_DAY)
    constexpr yearfunctions days{{PER_DAY(PUZZLE_PAIR)}};
    yearfunctions get_days() { return days; }
}
