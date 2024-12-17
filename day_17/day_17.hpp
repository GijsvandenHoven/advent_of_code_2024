#pragma once

#include <bitset>
#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 17

NAMESPACE_DEF(DAY) {

class Computer {
    int64_t A;
    int64_t B;
    int64_t C;
    std::ostringstream ss;
    int iptr = 0;
    std::array<std::function<void(int8_t)>, 8> opcodes = {
        [this](int8_t o){ this->adv(o); },
        [this](int8_t o){ this->bxl(o); },
        [this](int8_t o){ this->bst(o); },
        [this](int8_t o){ this->jnz(o); },
        [this](int8_t o){ this->bxc(o); },
        [this](int8_t o){ this->out(o); },
        [this](int8_t o){ this->bdv(o); },
        [this](int8_t o){ this->cdv(o); },
    };
    std::vector<int8_t> code;

public:
    Computer(): A(-1), B(-1), C(-1) {}

    Computer(const Computer& other): A(other.A), B(other.B), C(other.C), iptr(other.iptr), code(other.code) {
        ss.str(other.ss.str());
    }

    Computer& operator=(Computer&& other) noexcept {
        if (this != &other) {
            std::swap(A, other.A);
            std::swap(B, other.B);
            std::swap(C, other.C);
            ss = std::move(other.ss);
            std::swap(iptr, other.iptr);
            code = std::move(other.code);
        }

        return *this;
    }

    explicit Computer(std::ifstream& initialisation): A(-1), B(-1), C(-1) {
        {
            std::string line;
            std::getline(initialisation, line);
            std::stringstream ss (line);
            for (int i = 0; i < 12; ++i) {
                ss.get();
            }
            ss >> A;
        }
        {
            std::string line;
            std::getline(initialisation, line);
            std::stringstream ss (line);
            for (int i = 0; i < 12; ++i) {
                ss.get();
            }
            ss >> B;
        }
        {
            std::string line;
            std::getline(initialisation, line);
            std::stringstream ss (line);
            for (int i = 0; i < 12; ++i) {
                ss.get();
            }
            ss >> C;
        }

        if (A < 0 || B < 0 || C < 0) throw std::logic_error("Check parse");

        // program itself
        std::string line;
        std::getline(initialisation, line); // newline
        std::getline(initialisation, line); // program: (...)
        std::stringstream ss (line);
        for (int i = 0; i < 9; ++i) {
            ss.get();
        }

        while (ss.good()) {
            int val = ss.get(); // digit - every program consists of 1 digit operands.
            code.emplace_back(static_cast<int8_t>(val - '0'));

            int maybe_comma = ss.get(); // comma
            if (maybe_comma != ',') break;
        }
    }

    [[nodiscard]] int64_t get_combo_operand(uint8_t val) const {
        switch (val) {
            case 0: case 1: case 2: case 3:
                return val;
            case 4:
                return A;
            case 5:
                return B;
            case 6:
                return C;
            default: throw std::logic_error("Should not appear in valid programs");
        }
    }

    void adv(int8_t operand) {
        // std::cout << "adv\n";
        int64_t op = get_combo_operand(operand);
        A = A / (1 << op);
    }

    void bxl(int8_t operand) {
        // std::cout << "bxl\n";
        B = B ^ operand;
    }

    void bst(int8_t operand) {
        // std::cout << "bst\n";
        int64_t op = get_combo_operand(operand);
        B = op & (0b111);
    }

    void jnz(int8_t operand) {
        // std::cout << "jnz\n";
        if (A == 0) return;

        iptr = static_cast<uint8_t>(operand);
    }

    void bxc(int8_t /*legacy: ignores op*/) {
        // std::cout << "bxc\n";
        B = B ^ C;
    }

    void out(int8_t operand) {
        // std::cout << "out\n";
        int64_t op = get_combo_operand(operand);
        ss << (op & 0b111) << ","; // WARN: not to spec, last digit should not have a comma, truncate this on the output :)
    }

    void bdv(int8_t operand) {
        // std::cout << "bdv\n";
        int64_t op = get_combo_operand(operand);
        B = A / (1 << op);
    }

    void cdv(int8_t operand) {
        // std::cout << "cdv\n";
        int64_t op = get_combo_operand(operand);
        C = A / (1 << op);
    }

    void get_out(std::string& result) {
        result = ss.str();
        result = result.substr(0, result.size() - 1); // gross string copy, to deal with the last comma, surely the compiler will fix this for me :copium:
    }

    void reset(int64_t a = 0, int64_t b = 0, int64_t c = 0) {
        ss.clear();
        A = a;
        B = b;
        C = c;
        iptr = 0;
    }

    void execute() { // execute the program
        while (true) {
            // fetch
            // std::cout << "exec: '" << iptr << "', ";
            if (iptr < 0 || iptr >= code.size()) break; // fetch past program bounds.
            int8_t opcode = code[iptr];
            iptr++;

            // std::cout << static_cast<int>(opcode) << ", ";
            if (iptr < 0 || iptr >= code.size()) break; // fetch past program bounds.
            int8_t operand = code[iptr];
            iptr++; // jmp will handle the -2 for us dont worry about it.

            // std::cout << static_cast<int>(operand) << "...";
            if (opcode < 0 || opcode >= opcodes.size()) throw std::logic_error("unknown opcode");

            // execute
            opcodes[opcode](operand);
        }
    }

    // int64_t find_quine() {
    //     int64_t testing = 0;
    //     int64_t initB = B;
    //     int64_t initC = C;
    //
    //     std::vector<int8_t> out_so_far;
    //     int out_id = 0;
    //     // bruteforce for output. do early exits.
    //
    //     int best = 0;
    //
    //     while (true) {
    //         // set up execution here
    //         reset(testing, initB, initC);
    //         // std::cout << "try " << A << ", " << B << ", " << C << "\n";
    //         while (true) {
    //             // fetch
    //             // std::cout << "exec: '" << iptr << "', ";
    //             if (iptr < 0 || iptr >= code.size()) break; // fetch past program bounds.
    //             int8_t opcode = code[iptr];
    //             iptr++;
    //
    //             // std::cout << static_cast<int>(opcode) << ", ";
    //             if (iptr < 0 || iptr >= code.size()) break; // fetch past program bounds.
    //             int8_t operand = code[iptr];
    //             iptr++; // jmp will handle the -2 for us dont worry about it.
    //
    //             // std::cout << static_cast<int>(operand) << "...";
    //             if (opcode < 0 || opcode >= opcodes.size()) throw std::logic_error("unknown opcode");
    //
    //             // execute
    //             opcodes[opcode](operand);
    //
    //             if (opcode == 5) {
    //                 // std::cout << "did out " << last_out << "\n";
    //                 // check if last out matches code...
    //                 out_so_far.emplace_back(last_out);
    //                 if (out_so_far.back() != code[out_id]) {
    //                     if (out_id >= best) {
    //                         best = out_id;
    //                         std::cout << "matching best at " << testing << " outid " << out_id << "\n";
    //                     }
    //                     // reset
    //                     // std::cout << testing << " fails\n";
    //                     out_id = 0;
    //                     out_so_far.clear();
    //                     break;
    //                 } else {
    //                     ++out_id;
    //                 }
    //             }
    //         }
    //         // program terminated. is it a  quine?
    //         if (out_so_far == code) {
    //             break;
    //         } else {
    //             ++testing;
    //         }
    //     }
    //
    //
    //     return testing;
    // }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        c = std::move(Computer(input));
    }

    static void accurate_sim(std::string& out, const int64_t a) {
        out.clear();
        int64_t A = a;
        int64_t B = 0;
        int64_t C = 0;
        do {
            B = A & 0b111;
            B ^= 1;
            C = A >> (B);
            A >>= 3;
            B ^= C;
            B ^= 6;
            // out *= 10; // could also do octal
            out += std::to_string(B & 0b111);
        } while (A != 0);
    }

    static void print_next(int64_t start, int amount) {
        std::ofstream o ("d17_analysis.txt");

        for (int64_t i = start; i < start + amount; ++i) {

            std::string out;
            accurate_sim(out, i);

            o << i << ": " << out << "\n";
        }
    }

    void check(const std::string& output, int64_t lowest_A) const {
        // 343165530 should equal 118179096 if the formula is to be trusted.
        std::cout << "check... " <<  lowest_A << " is the lowest value that yields " << output << "?\n";

        Computer copy = c;
        copy.reset(lowest_A, 0, 0);
        copy.execute();
        std::string computer_check;
        copy.get_out(computer_check);
        std::cout << "PC verification: " << computer_check << "\n";

        for (int64_t i = 0; i <= lowest_A; ++i) {
            if (i % 100'000'000 == 0) std::cout << i << "\n";

            std::string out;
            accurate_sim(out, i);

            if (out == output) {
                std::cout << "match at " << i << "\n";
                if (i != lowest_A) {
                    throw std::logic_error("This is not valid.");
                }

                std::cout << lowest_A << " is the lowest value that yields " << output << "\n";
                return;
            }
        }

        throw std::logic_error("Loop ended not equal\n");
    }

    static int64_t numerical_analysis(int64_t value, int64_t seeking) {

        std::cout << "starting from " << value << " We want to find " << seeking << "\n";

        int64_t conclusion = -1;

        for (int i = 0; i < 10; ++i) {
            int64_t A = value + i;
            int64_t B = 0;
            int64_t C = 0;
            int64_t out = 0;
            do {
                B = A & 0b111;
                B ^= 1;
                C = A >> (B);
                A >>= 3;
                B ^= C;
                B ^= 6;
                out *= 10; // could also do octal
                out += (B & 0b111);
            } while (A != 0);

            if (out == seeking) {
                std::cout << "\tgot " << out << " after " << i <<"\n";
                conclusion = value + i;
                break;
            }
        }

        if (conclusion < 0) throw std::logic_error("could not find.");

        return conclusion;
    }

    void v1() const override {
        Computer copy = c;
        copy.execute();
        std::string r;
        copy.get_out(r);
        reportSolution(r);
    }

    void v2() const override { // holy quine!!
        static constexpr int64_t goal = 2411750343165530LL;
        std::cout << "for reference: the goal is " << goal << "\n";
        std::vector<int64_t> milestones;
        milestones.emplace_back(numerical_analysis(0, goal % 10LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 100LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 1'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 10'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 100'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 1'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 10'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 100'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 1'000'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 10'000'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 100'000'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 1'000'000'000'000LL));
        // milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 10'000'000'000'000LL)); // suddenly cannot find.
        milestones.emplace_back(numerical_analysis(484061601100, goal % 10'000'000'000'000LL)); // fuck it, bruteforce.
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 100'000'000'000'000LL));
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % 1'000'000'000'000'000LL));

        static constexpr int64_t TEN_QUAD = 10'000'000'000'000'000LL;
        static_assert (goal % TEN_QUAD == goal);
        milestones.emplace_back(numerical_analysis(milestones.back() * 8, goal % TEN_QUAD));
        std::cout << "#" << milestones.size() << ": " << milestones.back() << "\n";

        //print_next(60507697584 * 8, 1'000'000LL); // interesting: 118179096LL has a match at 7 that may be causing a dead end, but there are also candidates at +39 onward. We may need a recursion to check all islands, but how far ahead to check for island per iteration. 512?
        // check(std::to_string(343165530), 118179096LL);

        // for (int i = 0; i < milestones.size(); ++i) {
        //     std::cout << i << ": ";
        //     std::bitset<50> as_binary = (milestones[i]);
        //     std::cout << as_binary << "\n";
        // }

        reportSolution(milestones.back());
    }

    void parseBenchReset() override {

    }

    private:
    Computer c = Computer();
};

} // namespace

#undef DAY