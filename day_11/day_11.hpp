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
        }

        void stoneBlink(int nstep, int64_t stone, std::vector<int64_t>& output, int goal) const {
            if (nstep == goal) {
                output.emplace_back(stone);
                return;
            }

            // rule 1: 0 to 1.
            if (stone == 0) {
                stoneBlink(nstep+1, 1, output, goal);
                return;
            }
            // rule 2
            auto sstr = std::to_string(stone);
            std::cout << "stoi: " << sstr << "\n";
            if (sstr.size() % 2 == 0) { // yuck
                stoneBlink(nstep+1, std::stoi(sstr.substr(0, sstr.size() / 2)), output, goal);
                stoneBlink(nstep+1, std::stoi(sstr.substr(sstr.size() / 2)), output, goal);
                return;
            }
            // rule 3
            stoneBlink(nstep + 1, stone * 2024, output, goal);
        }

        void v1() const override {
            std::vector<int64_t> result;
            const int goal = 25;
            for (auto s : stones) {
                stoneBlink(0, s, result, goal);
            }

            reportSolution(result.size());
        }

        void v2() const override {

        }

        void parseBenchReset() override {
            stones.clear();
        }

        private:
        std::vector<int> stones;
    };

} // namespace

#undef DAY