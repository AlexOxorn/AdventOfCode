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

#define YEAR 2021
#define DAY 24

namespace day24 {
    enum opcode {
        INP,
        ADD,
        MUL,
        DIV,
        MOD,
        EQL,
        SET
    };
    enum variable {
        W,
        X,
        Y,
        Z
    };

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

#define CALL_NEXT()                             \
    if (instruction_iter == instructions.end()) \
        goto end;                               \
    current_instruction = *instruction_iter++;  \
    goto* jump_table[current_instruction.op];

#define GET_SOURCE() \
    (current_instruction.literal ? current_instruction.source_lit : registers[current_instruction.source_reg])

    struct instruction {
        opcode op;
        variable dest;
        union {
            variable source_reg;
            int source_lit;
        };
        bool literal;
    };

    /* ================================================
     * BYTE CODE SIMULATION
     * ===============================================
     */
    template<stdr::forward_range InstructionRange, stdr::input_range InputRange>
    std::array<long, 4> simulate(InstructionRange instructions, InputRange inputs) {
        std::array<long, 4> registers{};
        auto instruction_iter = instructions.begin();
        auto inputs_iter = inputs.begin();
        static void* jump_table[] = {
               &&inp, &&add, &&mul, &&div, &&mod, &&eql, &&set};
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

    std::array<long, 4> simulate(const std::array<long, 14>& input) {
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

    void print_stack(long z) {
        for (long i = z; i > 0l; i /= 26l) {
            printf("%ld ", i % 26l);
        }
        printf("\n");
    }

    long new_z(bool a, long b, long c, long input, long z) {
        bool x = z % 26l != input - b;
        assert(a == x);
        z /= a ? 1l : 26l;
        z *= x ? 26l : 1l;
        z += x ? (input + c) : 0l;
        printf("\033[%dm", a ? 0 : 31);
        printf("\t%ld\n", z);
        print_stack(z);
        printf("\033[0m");
        return z;
    }

    void stack_simulate_rec(bool a, long b, long c, long input, std::stack<long>& s);

    long stack_simulate(bool a, long b, long c, long input, long z) {
        bool x = z % 26l != input - b;
        z = z * 26l + input + c;
        printf("\t%ld\n", z);
        return z;
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
        printf("std::array<long, 4> simulate(const std::array<char, 14>& input) {");
        printf("int w, x, y, z;"
               "auto in = input.begin();");
        for (const instruction& inst : instructions) {
            switch (inst.op) {
                case INP:
                    printf("%c = *in++", variable_name(inst.dest));
                    break;
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
     * MODEL ITERATION
     * ===============================================
     */
    using model_type = std::array<int, 14>;
    class model_iteration {
    public:
        using iteration_category = std::forward_iterator_tag;
        using value_type = model_type;
        using difference_type = int;
        using reference = value_type&;
        using const_reference = const value_type&;

    private:
        model_type val = {
               9, 9, 9, 9, 9, 9, 9,
               9, 9, 9, 9, 9, 9, 9};

    private:
        void increment(const model_type::reverse_iterator& r) {
            if (r == val.rend())
                return;
            if (*r == 1) {
                *r = 9;
                return increment(r + 1);
            }
            (*r)--;
        }

    public:
        model_iteration() = default;
        model_iteration(bool)
            : val{1, 1, 1, 1, 1, 1, 1,
                   1, 1, 1, 1, 1, 1, 1} {}

        value_type operator*() const {
            return val;
        }

        model_iteration& operator++() {
            increment(val.rbegin());
            return *this;
        }

        model_iteration operator++(int) {
            auto cpy = *this;
            ++*this;
            return cpy;
        }

        bool operator<=>(const model_iteration& other) const = default;
    };

    void puzzle1() {
        auto instruction_range = GET_STREAM(input, instruction);
        std::vector instructions(instruction_range.begin(), instruction_range.end());

        auto split = split_code(instructions);
        auto As_range = split | stdv::transform([](auto instructions) { return instructions[4].source_lit == 1; });
        auto Bs_range = split | stdv::transform([](auto instructions) { return instructions[5].source_lit; });
        auto Cs_range = split | stdv::transform([](auto instructions) { return instructions[15].source_lit; });

        std::vector<char> As(As_range.begin(), As_range.end());
        std::vector<int> Bs(Bs_range.begin(), Bs_range.end());
        std::vector<int> Cs(Cs_range.begin(), Cs_range.end());

        /*
                auto model_numbers = stdr::subrange(model_iteration(), model_iteration(true))
                       | stdv::filter([&](auto s) {
                             printf("\033[2J\033[1;1H");
                             for (auto x : s) {
                                 printf("%1d", x);
                             }
                             printf(": ");
                             auto Aitr = As.begin();
                             auto Bitr = Bs.begin();
                             auto Citr = Cs.begin();
                             int temp = std::accumulate(s.begin(), s.end(), 0, [&](int old_z, int input) {
                                 return new_z(*Aitr++, *Bitr++, *Citr++, input, old_z);
                             });
                             printf("%d\n", temp);
                             return temp == 0;
                         })
                       | stdv::take(1);
        */

        while (true) { //               -2          -5 -4    -1 +3 -8 +6
            std::array<long, 14> s{9,2,7,9,3,9,4,9,4,8,9,9,9,5};
//            for (int i = 0; i < 14; i++) {
//                s[i] = (rand() % 9 + 1);
//                printf("%ld", s[i]);
//            }
//            printf("\n");
            auto simulate1 = simulate(instructions, s)[3];
            printf("simulate bytecode: %ld\n", simulate1);

            auto simulate2 = simulate(s)[3];
            //            assert(simulate2 == simulate1);
            printf("simulate c translation: %ld\n", simulate2);

            auto Aitr = As.begin();
            auto Bitr = Bs.begin();
            auto Citr = Cs.begin();
            long isolated_0 = std::accumulate(s.begin(), s.end(), 0l, [&](long old_z, long input) {
                return new_z0(*Aitr++, *Bitr++, *Citr++, input, old_z);
            });
            //            assert(isolated_0 == simulate1);
            printf("new_z0: %ld\n", isolated_0);

            Aitr = As.begin();
            Bitr = Bs.begin();
            Citr = Cs.begin();
            long isolated_1 = std::accumulate(s.begin(), s.end(), 0l, [&](long old_z, long input) {
                return new_z(*Aitr++, *Bitr++, *Citr++, input, old_z);
            });
            //            assert(isolated_1 == simulate1);
            printf("new_z: %ld\n", isolated_1);
            break;
        }

        /*model_type best_model_number = *model_numbers.begin();
        printf("The best model number is ");
        for (int i : best_model_number) {
            printf("%1d", int(i));
        }
        printf("\n");*/
    }

    void puzzle2() {}

#undef CALL_NEXT
#undef GET_SOURCE
#undef PARSE_REG
} // namespace day24