#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#include <omp.h>

#define DAY 22

NAMESPACE_DEF(DAY) {

struct PRNG {
    int64_t value;

    static constexpr int64_t MODULUS = 16777216; // bit 24

    int64_t next() {
        value ^= value * 64;
        value %= MODULUS;
        value ^= value / 32;
        value %= MODULUS;
        value ^= value * 2048;
        value %= MODULUS;

        return value;
    }
};

struct PriceData {
    int8_t delta;
    int8_t price;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            initials.emplace_back(std::stoi(line));
        }
    }

    void v1() const override {
        auto copy = initials;
        for (auto& pp : copy) {
            for (int i = 0; i < 2000; ++i) {
                pp.next();
            }
        }
        reportSolution(std::accumulate(copy.begin(), copy.end(), 0LL, [](auto s, auto& v){ return s + v.value; }));
    }

    void v2() const override {
        // reduce the prng to a sequence
        auto copy = initials;
        std::vector<std::array<PriceData, 2000>> sequences;
        for (auto& pp : copy) {
            auto prev = static_cast<int8_t>(pp.value % 10);
            std::array<PriceData, 2000> sequence {};
            for (int i = 0; i < 2000; ++i) {
                auto digit = static_cast<int8_t>(pp.next() % 10);
                auto delta = static_cast<int8_t>(digit - prev);

                sequence[i] = PriceData(delta, digit);

                prev = digit;
            }
            sequences.emplace_back(sequence); // ide says std::move has no purpose? sadge, i guess its because stack allocation?
        }

        auto value_of_sequence = [](const std::array<int8_t, 4>& seq, const std::array<PriceData, 2000>& list) {
            int8_t value = 0; // although not finding is not exactly 0, for the purpose of this puzzle you can treat it as such.
            for (int i = 3; i < list.size(); ++i) {
                if (list[i].delta != seq[3]) continue;
                if (list[i-1].delta != seq[2]) continue;
                if (list[i-2].delta != seq[1]) continue;
                if (list[i-3].delta != seq[0]) continue;

                value = std::max(value, list[i].price);
            }

            return static_cast<int>(value);
        };

        int best_value = 0;
        for (int8_t i = -9; i <= 9; ++i) {
            for (int8_t j = -9; j <= 9; ++j) {
                std::cout << static_cast<int>(j) << "\n";
                for (int8_t k = -9; k <= 9; ++k) {
                    for (int8_t l = -9; l <= 9; ++l) {
                        std::array<int8_t, 4> seq = {{ i,j,k,l }};
                        int total_value_of_seq = 0;
                        for (const auto& s : sequences) {
                            total_value_of_seq += value_of_sequence(seq, s);
                        }
                        best_value = std::max(best_value, total_value_of_seq);
                    }
                }
            }
        }


        reportSolution(best_value);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<PRNG> initials;
};

} // namespace

#undef DAY