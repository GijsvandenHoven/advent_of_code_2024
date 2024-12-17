#pragma once

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
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        c = std::move(Computer(input));
    }

    void v1() const override {
        Computer copy = c;
        copy.execute();
        std::string r;
        copy.get_out(r);
        reportSolution(r);
    }

    void v2() const override { // holy quine!!
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    Computer c = Computer();
};

} // namespace

#undef DAY