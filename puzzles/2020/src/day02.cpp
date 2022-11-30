//
// Created by alexoxorn on 2021-12-01.
//

#include "../../../common.h"
#include <cstdio>
#include <algorithm>
#include <cstring>

#define DAY 01

namespace aoc2020::day02 {
    struct password_and_policy {
        int min;
        int max;
        char password[50]{};
        char rep;
    };

    std::istream& operator>>(std::istream& in, password_and_policy& pass) {
        std::string line;
        std::getline(in, line);
        memset(pass.password, 0, 50);
        sscanf(line.c_str(), "%d-%d %c: %50s", &pass.min, &pass.max, &pass.rep, pass.password);
        return in;
    }

    bool valid_password(password_and_policy pass) {
        auto instances = stdr::count(pass.password, pass.rep);
        bool valid = pass.min <= instances && instances <= pass.max;
        return valid;
    }

    bool valid_password2(password_and_policy pass) {
        return (pass.password[pass.min-1] == pass.rep) != (pass.password[pass.max-1] == pass.rep);
    }

    void puzzle1(const char* filename) {
        auto input_vector = get_stream<password_and_policy>(filename);
        auto valid_passwords = stdr::count_if(input_vector, valid_password);
        printf("The number of valid passwords is %zu\n", valid_passwords);
    }

    void puzzle2(const char* filename) {
        auto input_vector = get_stream<password_and_policy>(filename);
        auto valid_passwords = stdr::count_if(input_vector, valid_password2);
        printf("The number of true valid passwords is %zu\n", valid_passwords);
    }
} // namespace aoc2020::day02