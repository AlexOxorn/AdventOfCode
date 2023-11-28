//
// Created by alexoxorn on 10/29/23.
//

#ifndef ADVENTOFCODE_TESTER_H
#define ADVENTOFCODE_TESTER_H

#include "common.h"

struct answer_to_string {
    std::string operator()(std::integral auto i) { return std::to_string(i); }
    std::string operator()(const std::string& s) { return s; }
    std::string operator()(const std::monostate&) { return "(nil)"; }
};

struct answer_to_string2 {
    std::optional<std::string> operator()(std::integral auto i) { return std::to_string(i); }
    std::optional<std::string> operator()(const std::string& s) {
        std::stringstream ss;
        ss << std::quoted(s.c_str(), '\'');
        return ss.str();
    }
    std::optional<std::string> operator()(const std::monostate&) { return {}; }
};

void test(int year, const yearfunctions& functions, const std::string& answer_key_path);

#endif // ADVENTOFCODE_TESTER_H
