#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 11

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string line;
            std::getline(input, line);
            std::stringstream ss(line);
            int v;
            while (!ss.eof()) {
                ss >> v;
                stones.emplace_back(v);
            }

            for (auto& x : stones) {
                std::cout << x << "\n";
            }
        }

        void v1() const override {
            reportSolution(0);
        }

        void v2() const override {
            reportSolution(0);
        }

        void parseBenchReset() override {
            stones.clear();
        }

        private:
        std::vector<int> stones;
    };

} // namespace

#undef DAY