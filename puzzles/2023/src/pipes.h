

#ifndef ADVENTOFCODE_PIPES_H
#define ADVENTOFCODE_PIPES_H

#include "../../../common.h"
#include <cstdint>
#include <ox/grid.h>
#include <algorithm>
#include <bit>
#include <ranges>
#include <unordered_set>
#include <deque>

enum PipeDir : uint8_t {
    Start = (1 << 5),
    MainLoop = (1 << 4),
    Outside = (1 << 6),
    Inside = (1 << 7),
    FLAGS = Start | MainLoop | Outside | Inside,
    None = 0,
    N = (1 << 0),
    S = (1 << 1),
    E = (1 << 2),
    W = (1 << 3),

    DIRECTION = N | S | E | W,
    NW = N | W,
    NE = N | E,
    SW = S | W,
    SE = S | E,
    V = N | S,
    H = E | W,
};

inline const char* pipe_chars(uint8_t p) {
    switch (p & DIRECTION) {
        case Start: return "S";
        case V: return "║";
        case H: return "═";
        case NE: return "╚";
        case NW: return "╝";
        case SE: return "╔";
        case SW: return "╗";
        case None: return "▒";
        default: return "X";
    }
}

inline const char* color(uint8_t d) {
    if (Start & d)
        return "32";
    if (MainLoop & d)
        return "31";
    if (Inside & d)
        return "34";
    if (Outside & d)
        return "30";
    return "0";
}


struct pipe_structure : ox::grid<uint8_t> {
    using ox::grid<uint8_t>::grid;
    using ox::grid<uint8_t>::const_raw_iterator;

    auto calculate_start() {
        auto start_pos = stdr::find(data, Start);
        uint8_t matching_position = None;
        if (auto x = up(std::optional(start_pos)); x && **x & S) {
            matching_position |= N;
        }
        if (auto x = left(std::optional(start_pos)); x && **x & E) {
            matching_position |= W;
        }
        if (auto x = down(std::optional(start_pos)); x && **x & N) {
            matching_position |= S;
        }
        if (auto x = right(std::optional(start_pos)); x && **x & W) {
            matching_position |= E;
        }
        *start_pos = PipeDir(*start_pos | matching_position);
        return start_pos;
    }

    static uint8_t next_direction(uint8_t curr, uint8_t from) {
        // NORTH-WEST PIPE
        // COMING FROM NORTH
        // GO WEST
        // NW XOR N => W
        // 1001 xor 0001 => 1000
        return DIRECTION & (curr ^ from);
    }

    long traverse_path(const raw_iterator& start) {
        uint8_t from = std::bit_floor(uint8_t(DIRECTION & *start));
        long distance = 0;
        auto head = std::optional(start);
        *start |= 1 << 5;
        do {
            ++distance;
            **head = PipeDir(MainLoop | **head);
            switch (next_direction(**head, from)) {
                case N:
                    head = up(head);
                    from = S;
                    break;
                case S:
                    head = down(head);
                    from = N;
                    break;
                case E:
                    head = right(head);
                    from = W;
                    break;
                case W:
                    head = left(head);
                    from = E;
                    break;
                default: std::unreachable();
            }
        } while (head != start);
        return distance / 2;
    }

    void within_loop(const raw_iterator& candidate) {
        std::unordered_set<long> group;
        std::queue<raw_iterator> Q;
        Q.push(candidate);
        group.insert(candidate - data.begin());

        bool inbounds = true;
        while (!Q.empty()) {
            auto top = Q.front();
            Q.pop();
            auto cardinal = neighbour_range(top);
            for (auto item : cardinal) {
                if (!item || (**item & Outside)) {
                    inbounds = false;
                    break;
                }
                if (!(**item & MainLoop) && !(group.contains(*item - data.begin()))) {
                    Q.push(*item);
                    group.insert(*item - data.begin());
                }
            }
        }

        for (auto index : group) {
            data[index] = PipeDir(data[index] | (inbounds ? Inside : Outside));
        }
    }

    auto move_dir(std::optional<raw_iterator> head, uint8_t d) {
        switch (d) {
            case N: return up(head); break;
            case S: return down(head); break;
            case E: return right(head); break;
            case W: return left(head); break;
            default: std::unreachable();
        }
    }

    static auto opposite(uint8_t from) {
        switch (from) {
            case N: return S; break;
            case S: return N; break;
            case E: return W; break;
            case W: return E; break;
            default: std::unreachable();
        };
    }
    auto get_right(std::optional<raw_iterator> head, uint8_t back) {
        switch (back) {
            case N: return left(head); break;
            case S: return right(head); break;
            case E: return up(head); break;
            case W: return down(head); break;
            default: std::unreachable();
        };
    }

    static bool is_moving_left(std::optional<raw_iterator> head, uint8_t from) {
        switch (from) {
            case N: return (DIRECTION & **head) == NE; break;
            case S: return (DIRECTION & **head) == SW; break;
            case E: return (DIRECTION & **head) == SE; break;
            case W: return (DIRECTION & **head) == NW; break;
            default: std::unreachable();
        }
    }

    void right_hand_rule() {
        auto topleftpipe = std::optional(stdr::find_if(data, [](uint8_t d) { return d & MainLoop; }));
        uint8_t from = next_direction(**topleftpipe, S);
        auto head = std::optional(topleftpipe);
        do {
            switch (next_direction(**head, from)) {
                case N:
                    head = up(head);
                    from = S;
                    break;
                case S:
                    head = down(head);
                    from = N;
                    break;
                case E:
                    head = right(head);
                    from = W;
                    break;
                case W:
                    head = left(head);
                    from = E;
                    break;
                default: std::unreachable();
            }
            auto right = get_right(head, from);
            if (right && !(**right & MainLoop)) {
                **right = PipeDir(Outside | **right);
            }
            if (is_moving_left(head, from)) {
                PipeDir cont = opposite(from);
                if (auto forward = move_dir(head, cont); forward && !(**forward & MainLoop)) {
                    **forward = PipeDir(Outside | **forward);
                }
                if (auto diagonal = move_dir(right, cont); diagonal && !(**diagonal & MainLoop)) {
                    **diagonal = PipeDir(Outside | **diagonal);
                }
            }
        } while (head != topleftpipe);
    }

    void check_remaining_bounds() {
        auto start = data.begin();
        auto end = data.end();
        for (; start != end; ++start) {
            if (*start & (MainLoop | Inside | Outside)) {
                continue;
            }
            within_loop(start);
        }
    }

    long count_bounded() {
        right_hand_rule();
        check_remaining_bounds();
        return stdr::count_if(data, [](uint8_t d) { return d & Inside; });
    }

    void print() {
        leveled_foreach([](uint8_t c) { myprintf("\033[%sm%s", color(c), pipe_chars(c)); },
                        []() { myprintf("\n"); });
        myprintf("\033[0m\n");
    }
};

#endif // ADVENTOFCODE_PIPES_H
