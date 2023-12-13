#include <array>
#include "../../../common.h"

namespace aoc2023 {
    PER_DAY(INCLUDE_DAY)
    constexpr yearfunctions days{{PER_DAY(PUZZLE_PAIR)}};
    yearfunctions get_days() { return days; }
}

