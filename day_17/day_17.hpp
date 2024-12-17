#pragma once

#include <bitset>
#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 17

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        // c = std::move(Computer(input));
    }

    static int64_t fast_simulate(const int64_t a) {
            int64_t A = a;
            int64_t out = 0;
            do {
                int64_t B = A & 0b111;
                B ^= 1;
                int64_t C = A >> (B);
                A >>= 3;
                B ^= C;
                B ^= 6;
                out *= 10; // could also do octal
                out += (B & 0b111);
            } while (A != 0);

        return out;
    }

    static int64_t numerical_analysis(int64_t value, int64_t seeking) {
        // std::cout << "from " << value << " seek " << seeking << "\n";

        for (int i = 0; i < 8; ++i) {
            if (const int64_t out = fast_simulate(value + i); out == seeking) {
                return value + i;
            }
        }

        // roblem: not found in the next 8 digits, doing a brute force. This does not work on a general solution, but it works here :3
        // normally you should find all the "islands" per iteration and recurse with that * 8 as the starting point to check, one of the islands should lead here.
        int64_t i = 0;
        while (true) {
            if (const int64_t out = fast_simulate(value + i); out == seeking) {
                return value + i;
            }

            ++i;
        }
    }

    void v1() const override {
        int64_t result = fast_simulate(18427963);
        int magnitude = 1;
        while ( result / magnitude ) magnitude *= 10;

        std::stringstream ss;
        auto update = [&] {
            magnitude /= 10;
            ss << result / magnitude;
            result %= magnitude;
        };

        update();
        while (result) {
            ss << ",";
            update();
        }


        reportSolution(ss.str());
    }

    void v2() const override { // holy quine!!
        // const int64_t goal = c.code_to_number(); // would work for general case if you parse in a Computer instance in the parse step. Because of the bruteforcing rather than tracking islands with recursion, one could construct a program that bruteforces too much to complete in meaningful time! (pump the '5,7' part of the program)
        constexpr int64_t goal = 2411750343165530LL;

        int64_t index = 0;
        for (int64_t mod = 10; mod < goal; mod *= 10) {
            index = numerical_analysis(index * 8, goal % mod);
        }

        // last step: we know where all but the first digit of the quine will repeat in a quine-digited number for the first time, so seek from there.
        reportSolution(numerical_analysis(index * 8, goal));
    }

    void parseBenchReset() override {

    }
};

} // namespace

#undef DAY