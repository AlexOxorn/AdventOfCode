//
// Created by alexoxorn on 2021-12-02.
//

#ifndef ADVENTOFCODE2021_COMMON_H
#define ADVENTOFCODE2021_COMMON_H

#include <fstream>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <optional>
#include <vector>
#include <ranges>
#include <variant>
#include <ox/io.h>
#include <ox/std_abbreviation.h>

using namespace ox::std_abbreviations;

#define XSTR(a) STR(a)
#define STR(a)  #a

extern long year, day;
extern bool do_print;

#define myprintf(...) (do_print ? printf(__VA_ARGS__) : 0)

template <typename T = ox::line>
auto get_stream(const char* name) {
    char filename[512];
    sprintf(filename, "%s/../puzzles/%ld/inputs/day%02ld_%s.txt", ox::executable_folder().c_str(), year, day, name);
    return ox::ifstream_container<T>{filename};
}

template <typename T, typename C = std::vector<T>>
C get_from_input(const char* filename) {
    static std::optional<C> input_vector;
    if (!input_vector) {
        auto ss = get_stream<T>(filename);
        input_vector.emplace(std::begin(ss), std::end(ss));
    }
    return input_vector.value();
}

#define DEFINE_VECTOR_FROM_ISTREAM_INPUT_METHOD(name, type) \
  std::vector<type> get_##name() { \
    static std::optional<std::vector<(type)>> input_vector; \
    if (!input_vector) { \
      auto ss = get_stream<type>("name"); \
      input_vector.emplace(std::begin(ss), std::end(ss)); \
    } \
    return input_vector.value(); \
  }

#define DEFINE_DEFAULT_GET_VECTORS(type) \
  DEFINE_VECTOR_FROM_ISTREAM_INPUT_METHOD(input, type) \
  DEFINE_VECTOR_FROM_ISTREAM_INPUT_METHOD(sample_input, type)

#define PER_DAY(MACRO) \
  MACRO(01) \
  MACRO(02) \
  MACRO(03) \
  MACRO(04) \
  MACRO(05) \
  MACRO(06) \
  MACRO(07) \
  MACRO(08) \
  MACRO(09) \
  MACRO(10) \
  MACRO(11) \
  MACRO(12) \
  MACRO(13) \
  MACRO(14) \
  MACRO(15) \
  MACRO(16) \
  MACRO(17) \
  MACRO(18) \
  MACRO(19) \
  MACRO(20) \
  MACRO(21) \
  MACRO(22) \
  MACRO(23) \
  MACRO(24) \
  MACRO(25)

using answertype = std::variant<std::monostate, long, unsigned long, std::string>;

using puzzle_sig = answertype (*)(const char*);
using dayfunctions = std::pair<puzzle_sig, puzzle_sig>;
using yearfunctions = std::array<dayfunctions, 25>;

#define PUZZLE_PAIR(X) {day##X::puzzle1, day##X::puzzle2},
#define INCLUDE_DAY(X) \
  namespace day##X { \
    COMMON_HEADER \
  }

#define COMMON_HEADER \
  answertype puzzle1(const char*); \
  answertype puzzle2(const char*);

#endif // ADVENTOFCODE2021_COMMON_H
