//
// Created by alexoxorn on 2021-12-31.
//

#include "day24.h"
#include <concepts>
#include <optional>
#include <ox/debug.h>
#include <numeric>
#include <stack>
#include <cassert>
#include <ranges>

#define YEAR 2021
#define DAY  24

namespace day24 {
    enum opcode { INP, ADD, MUL, DIV, MOD, EQL, SET };
    enum variable { W, X, Y, Z };
    struct instruction {
        opcode op;
        variable dest;
        union {
            variable source_reg;
            int source_lit;
        };
        bool literal;
    };

    static std::vector<instruction> instructions;
    static std::vector<char> As;
    static std::vector<int> Bs;
    static std::vector<int> Cs;

#define CALL_NEXT() \
  if (instruction_iter == instructions.end()) \
    goto end; \
  current_instruction = *instruction_iter++; \
  goto* jump_table[current_instruction.op];

#define GET_SOURCE() \
  (current_instruction.literal ? current_instruction.source_lit : registers[current_instruction.source_reg])

    const char* opcode_name(opcode op) {
        switch (op) {
            case INP: return "inp";
            case ADD: return "add";
            case MUL: return "mul";
            case DIV: return "div";
            case MOD: return "mod";
            case EQL: return "eql";
            case SET: return "set";
        }
        return nullptr;
    }

    char variable_name(variable v) {
        switch (v) {
            case W: return 'w';
            case X: return 'x';
            case Y: return 'y';
            case Z: return 'z';
        }
        return 0;
    }

    opcode from_char(const char type[]) {
        if (type[0] == 'i')
            return INP;
        if (type[0] == 'a')
            return ADD;
        if (type[0] == 'd')
            return DIV;
        if (type[0] == 'e')
            return EQL;
        if (type[0] == 'm' && type[1] == 'u')
            return MUL;
        if (type[0] == 'm' && type[1] == 'o')
            return MOD;
        return SET;
    }

    /* ================================================
     * BYTE CODE SIMULATION
     * ===============================================
     */
    template <stdr::forward_range InstructionRange, stdr::input_range InputRange>
    std::array<long, 4> simulate(InstructionRange instructions, InputRange inputs) {
        std::array<long, 4> registers{};
        auto instruction_iter = instructions.begin();
        auto inputs_iter = inputs.begin();
        static void* jump_table[] = {&&inp, &&add, &&mul, &&div, &&mod, &&eql, &&set};
        instruction current_instruction{};

        CALL_NEXT();
    inp:
        registers[current_instruction.dest] = *inputs_iter++;
        CALL_NEXT();
    add:
        registers[current_instruction.dest] += GET_SOURCE();
        CALL_NEXT();
    mul:
        registers[current_instruction.dest] *= GET_SOURCE();
        CALL_NEXT();
    div:
        registers[current_instruction.dest] /= GET_SOURCE();
        CALL_NEXT();
    mod:
        registers[current_instruction.dest] %= GET_SOURCE();
        CALL_NEXT();
    eql:
        registers[current_instruction.dest] = registers[current_instruction.dest] == GET_SOURCE();
        CALL_NEXT();
    set:
        registers[current_instruction.dest] = GET_SOURCE();
        CALL_NEXT();
    end:
        return registers;
    }

    std::array<long, 4> simulate(stdr::random_access_range auto& input) {
        long w = 0, x = 0, y = 0, z = 0;
        auto in = input.begin();
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 11;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 8;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 14;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 13;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 10;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 2;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += 0;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 7;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 11;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 4;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 13;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -8;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 13;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -9;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 10;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 1;
        x += 11;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 1;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += 0;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 2;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -5;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 14;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -6;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 6;
        y *= x;
        z += y;
        w = *in++;
        x *= 0;
        x += z;
        x %= 26;
        z /= 26;
        x += -12;
        x = x == w;
        x = x == 0;
        y *= 0;
        y += 25;
        y *= x;
        y += 1;
        z *= y;
        y *= 0;
        y += w;
        y += 14;
        y *= x;
        z += y;
        return {w, x, y, z};
    }

    long new_z0(bool a, long b, long c, long input, long z) {
        long w, x, y;
        w = input;
        x *= 0l;
        x += z;
        x %= 26l;
        z /= a ? 1l : 26l;
        x += b;
        x = x == w;
        x = x == 0;
        y *= 0l;
        y += 25l;
        y *= x;
        y += 1;
        z *= y;
        y *= 0l;
        y += w;
        y += c;
        y *= x;
        z += y;
        return z;
    }

    long new_raw(bool a, long b, long c, long input, long z) {
        bool x = z % 26l != input - b;
        z /= a ? 1l : 26l;
        z *= x ? 26l : 1l;
        z += x ? (input + c) : 0l;
        return z;
    }

    void print_stack(long z) {
        for (long i = z; i > 0l; i /= 26l) {
            printf("%ld ", i % 26l);
        }
        printf("\n");
    }

    long new_z_debug(bool a, long b, long c, long input, long z) {
        long next = new_raw(a, b, c, input, z);
        printf("\033[%dm", a ? 0 : 31);
        printf("\t%ld\n", next);
        print_stack(z);
        printf("\033[0m");
        return z;
    }

    template <bool Min = false>
    std::vector<long> get_valid_input(const std::vector<char>& a, const std::vector<int>& b,
                                      const std::vector<int>& c) {
        std::stack<std::pair<int, int>> s;
        std::vector<long> to_return(a.size());
        for (auto [aItr, bItr, cItr] = std::make_tuple(a.begin(), b.begin(), c.begin()); aItr != a.end();
             ++aItr, ++bItr, ++cItr) {
            if (*aItr) {
                s.emplace(*cItr, aItr - a.begin());
            } else {
                auto [c_val, p_index] = s.top();
                long c_index = aItr - a.begin();
                s.pop();

                int sum = *bItr + c_val;
                if constexpr (Min) {
                    if (sum < 0) {
                        assert(sum > -9);
                        to_return[p_index] = 1 - sum;
                        to_return[c_index] = 1;
                    } else if (sum == 0) {
                        to_return[p_index] = 1;
                        to_return[c_index] = 1;
                    } else if (sum > 0) {
                        assert(sum < 9);
                        to_return[p_index] = 1;
                        to_return[c_index] = 1 + sum;
                    }
                } else {
                    if (sum < 0) {
                        assert(sum > -9);
                        to_return[p_index] = 9;
                        to_return[c_index] = 9 + sum;
                    } else if (sum == 0) {
                        to_return[p_index] = 9;
                        to_return[c_index] = 9;
                    } else if (sum > 0) {
                        assert(sum < 9);
                        to_return[p_index] = 9 - sum;
                        to_return[c_index] = 9;
                    }
                }
            }
        }
        return to_return;
    }

    /* ================================================
     * BYTE CODE SPLITTING
     * ===============================================
     */

    std::vector<std::vector<instruction>> split_code(const std::vector<instruction>& instructions) {
        std::vector<std::vector<instruction>> to_return;
        auto itr = instructions.begin();
        while (itr != instructions.end()) {
            std::vector<instruction> part;
            do {
                part.push_back(*itr++);
                if (itr == instructions.end())
                    break;
            } while (itr->op != INP);
            to_return.push_back(std::move(part));
        }
        return to_return;
    }

    /* ================================================
     * BYTE CODE PRINTING
     * ===============================================
     */

    void print_instruction(const instruction& inst) {
        printf("%s %c", opcode_name(inst.op), variable_name(inst.dest));
        if (inst.op != INP) {
            if (inst.literal)
                printf(" %d", inst.source_lit);
            else
                printf(" %c", variable_name(inst.source_reg));
        }
    }

    void print_instructions(const std::vector<instruction>& instructions) {
        for (const instruction& inst : instructions) {
            print_instruction(inst);
            printf("\n");
        }
    }

    void print_as_c(const std::vector<instruction>& instructions) {
        printf("std::array<long, 4> simulate(const std::array<char, 14>& "
               "input) {");
        printf("int w, x, y, z;"
               "auto in = input.begin();");
        for (const instruction& inst : instructions) {
            switch (inst.op) {
                case INP: printf("%c = *in++", variable_name(inst.dest)); break;
                case ADD:
                    printf("%c +=", variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
                case MUL:
                    printf("%c *=", variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
                case DIV:
                    printf("%c /=", variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
                case MOD:
                    printf("%c %%=", variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
                case EQL:
                    printf("%c = %c == ", variable_name(inst.dest), variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
                case SET:
                    printf("%c =", variable_name(inst.dest));
                    inst.literal ? printf(" %d", inst.source_lit) : printf(" %c", variable_name(inst.source_reg));
                    break;
            }
            printf(";");
        }
        printf("return {w, x, y, z};}");
    }

    /* ================================================
     * FILE PARSING
     * ===============================================
     */
    std::istream& operator>>(std::istream& in, instruction& inst) {
        std::string s;
        std::getline(in, s);
        char c1, c2;
        int i;
        char inst_name[4];

        if (sscanf(s.c_str(), "inp %c", &c1)) {
            inst.op = INP;
            inst.dest = static_cast<variable>(c1 - 'w');
            return in;
        }
        if (sscanf(s.c_str(), "%s %c %d", inst_name, &c1, &i) == 3) {
            inst.op = from_char(inst_name);
            inst.dest = static_cast<variable>(c1 - 'w');
            inst.source_lit = i;
            inst.literal = true;
        } else {
            sscanf(s.c_str(), "%s %c %c", inst_name, &c1, &c2);
            inst.op = from_char(inst_name);
            inst.dest = static_cast<variable>(c1 - 'w');
            inst.source_reg = static_cast<variable>(c2 - 'w');
            inst.literal = false;
        }

        return in;
    }

    /* ================================================
     * TESTING
     * ===============================================
     */
    void print_answer(const std::vector<long>& answer) {
        for (int i = 0; i < 14; i++) {
            printf("%ld", answer[i]);
        }
        printf("\n");
    }

    void test_answer(const std::vector<long>& answer) {
        printf("Testing: ");
        print_answer(answer);

        auto simulate1 = simulate(instructions, answer)[3];
        printf("simulate bytecode: %ld\n", simulate1);

        auto simulate2 = simulate(answer)[3];
        assert(simulate2 == simulate1);
        printf("simulate c translation: %ld\n", simulate2);

        auto Aitr = As.begin();
        auto Bitr = Bs.begin();
        auto Citr = Cs.begin();
        long isolated_0 = std::accumulate(answer.begin(), answer.end(), 0l, [&](long old_z, long input) {
            return new_z0(*Aitr++, *Bitr++, *Citr++, input, old_z);
        });
        assert(isolated_0 == simulate1);
        printf("new_z0: %ld\n", isolated_0);

        Aitr = As.begin();
        Bitr = Bs.begin();
        Citr = Cs.begin();
        long isolated_1 = std::accumulate(answer.begin(), answer.end(), 0l, [&](long old_z, long input) {
            return new_raw(*Aitr++, *Bitr++, *Citr++, input, old_z);
        });
        assert(isolated_1 == simulate1);
        printf("new_raw: %ld\n", isolated_1);
    }

    void init() {
        auto instruction_range = GET_STREAM(input, instruction);
        instructions = std::vector(instruction_range.begin(), instruction_range.end());

        auto split = split_code(instructions);
        auto As_range = split | stdv::transform([](auto instructions) { return instructions[4].source_lit == 1; });
        auto Bs_range = split | stdv::transform([](auto instructions) { return instructions[5].source_lit; });
        auto Cs_range = split | stdv::transform([](auto instructions) { return instructions[15].source_lit; });

        As = std::vector<char>(As_range.begin(), As_range.end());
        Bs = std::vector<int>(Bs_range.begin(), Bs_range.end());
        Cs = std::vector<int>(Cs_range.begin(), Cs_range.end());
    }
    void puzzle1() {
        init();
        auto answer = get_valid_input<false>(As, Bs, Cs);
        print_answer(answer);
    }

    void puzzle2() {
        auto answer = get_valid_input<true>(As, Bs, Cs);
        print_answer(answer);
    }

#undef CALL_NEXT
#undef GET_SOURCE
#undef PARSE_REG
} // namespace day24