//
// Created by alexoxorn on 2021-12-19.
//

#include "day16.h"
#include <vector>
#include <bitset>
#include <iterator>
#include <variant>
#include <numeric>
#include <functional>

#define YEAR 2021
#define DAY 16

namespace day16 {
    struct packet;
    struct Literal {
        std::vector<bool> number;
    };
    struct Operator {
        int length;
        std::vector<packet> packets;
        bool lengthId;
    };
    struct packet {
        int version;
        int type;
        std::variant<Literal, Operator> body;
    };

    template <std::forward_iterator T>
    unsigned long parse_int_from_bool(T begin, T end) {
        return std::accumulate(begin, end, 0ul,
               [](auto acc, auto bit) { return (acc << 1) | bit; });
    }

    template <std::forward_iterator T>
    std::pair<packet, T> parse_packet(T current_position);

    template <std::forward_iterator T>
    std::pair<Literal, T> parse_literal(T current_position) {
        Literal l;
        bool cont = true;
        while(cont) {
            cont = *current_position++;
            std::copy(current_position, current_position + 4, std::back_inserter(l.number));
            current_position += 4;
        }
        return {l, current_position};
    }

    template <std::forward_iterator T>
    std::pair<Operator, T> parse_operator(T current_position) {
        Operator o;
        o.lengthId = *current_position++;
        if (o.lengthId) {
            o.length = parse_int_from_bool(current_position, current_position + 11);
            current_position += 11;
            for (int i : stdv::iota(0, o.length)) {
                auto [packet, new_position] = parse_packet(current_position);
                o.packets.push_back(std::move(packet));
                current_position = new_position;
            }
        } else {
            o.length = parse_int_from_bool(current_position, current_position + 15);
            current_position += 15;
            auto endpoint = current_position + o.length;
            while (current_position != endpoint) {
                auto [packet, new_position] = parse_packet(current_position);
                o.packets.push_back(std::move(packet));
                current_position = new_position;
            }
        }
        return {o, current_position};
    }

    template <std::forward_iterator T>
    std::pair<packet, T> parse_packet(T current_position) {
        packet p;
        p.version = parse_int_from_bool(current_position, current_position + 3);
        current_position += 3;
        p.type = parse_int_from_bool(current_position, current_position + 3);
        current_position += 3;
        if (p.type == 4) {
            auto [literal, new_position] = parse_literal(current_position);
            p.body = literal;
            current_position = new_position;
        } else {
            auto [op, new_position] = parse_operator(current_position);
            p.body = op;
            current_position = new_position;
        }
        return {p, current_position};
    }

    long evaluate_packet(const packet& p);

    char type_to_op(int i) {
        switch (i) {
            case 0: return '+';
            case 1: return '*';
            case 2: return '_';
            case 3: return '^';
            case 4: return '#';
            case 5: return '>';
            case 6: return '<';
            case 7: return '=';
            default: return 0;
        }
    }

    std::function<long(long, packet)> type_to_reduce(int i) {
        switch (i) {
            case 0: return [](long i, const packet& p) { return i + evaluate_packet(p); };
            case 1: return [](long i, const packet& p) { return i * evaluate_packet(p); };
            case 2: return [](long i, const packet& p) { return std::min(i, evaluate_packet(p)); };
            case 3: return [](long i, const packet& p) { return std::max(i, evaluate_packet(p)); };
            case 5: return [](long i, const packet& p) { return i > evaluate_packet(p); };
            case 6: return [](long i, const packet& p) { return i < evaluate_packet(p); };
            case 7: return [](long i, const packet& p) { return i == evaluate_packet(p); };
            default: return [](long, const packet& p) { return 0l; };
        }
    }

    long evaluate_literal(const Literal& l) {
        return parse_int_from_bool(l.number.begin(), l.number.end());
    }
    long evaluate_operator(const Operator& o, int type) {
        return std::accumulate(std::next(o.packets.begin()), o.packets.end(), evaluate_packet(o.packets.front()), type_to_reduce(type));
    }
    long evaluate_packet(const packet& p) {
        if (p.type == 4) {
            return evaluate_literal(std::get<0>(p.body));
        } else {
            return evaluate_operator(std::get<1>(p.body), p.type);
        }
    }

    void print_packet(const packet& p, int indent = 0);
    void print_literal(const Literal& l, int indent = 0) {
        printf("(%lu)", parse_int_from_bool(l.number.begin(), l.number.end()));
    }
    void print_operator(const Operator& o, int indent = 0) {
        for (packet p : o.packets) {
            printf("\n");
            print_packet(p, indent + 1);
        }
    }
    void print_packet(const packet& p, int indent) {
        char temp[10];
        sprintf(temp, "%%%ds", indent * 4);
        printf(temp, "");
        printf("<version = %d, type = '%c'> ", p.version, type_to_op(p.type));
        if (p.type == 4) {
            print_literal(std::get<0>(p.body));
        } else {
            print_operator(std::get<1>(p.body), indent);
        }
    }

    long add_packet_version(const packet& p);
    long add_operator_version(const Operator& o) {
        return std::accumulate(o.packets.begin(), o.packets.end(), 0l, [](long acc, const packet& p) { return acc + add_packet_version(p); });
    }
    long add_packet_version(const packet& o) {
        int acc = o.version;
        if (o.type != 4) {
            acc += add_operator_version(std::get<1>(o.body));
        }
        return acc;
    }

    template <std::ranges::range R>
    std::vector<bool> parse_character(R& input) {
        std::vector<bool> b;
        for (char c : input) {
            char str[2] = {c, 0};
            long num = strtol(str, nullptr, 16);
            auto x = stdv::iota(0u, 4u) | stdv::transform([num](int i) { return static_cast<bool>((1u << i) & num); }) | stdv::reverse;
            std::copy(x.begin(), x.end(), std::back_inserter(b));
        }
        return b;
    }

    void puzzle1() {
        auto input = GET_STREAM(input, char);
        std::vector<bool> b = parse_character(input);
        auto [packet, end] = parse_packet(b.begin());
        printf("The total version count is: %ld\n", add_packet_version(packet));
    }

    void puzzle2() {
        auto input = GET_STREAM(input, char);
        std::vector<bool> b = parse_character(input);
        auto [packet, end] = parse_packet(b.begin());
        printf("The total version count is: %ld\n", evaluate_packet(packet));
    }
}