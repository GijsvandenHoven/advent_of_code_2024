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

        int64_t countSplits(int64_t stone, int g_blinks, std::vector<std::map<int64_t, int64_t>>& cache) const {

            if (cache[g_blinks].contains(stone)) {
                // std::cout << " cache hit, saving " << g_blinks << "\n";
                return cache[g_blinks].find(stone)->second;
            }

            // base case
            if (g_blinks == 0) {
                cache[0].emplace(stone, 1);
                return 1; // this stone is part of the final count.
            }

            // step 1
            if (stone == 0) {
                int64_t result = countSplits(1, g_blinks - 1, cache);
                cache[g_blinks].emplace(stone, result);

                return result;
            }

            // step 2
            auto sstr = std::to_string(stone);
            if (sstr.size() % 2 == 0) { // yuck
                auto left = std::stoi(sstr.substr(0, sstr.size() / 2));
                auto right = std::stoi(sstr.substr(sstr.size() / 2));
                int64_t result = countSplits(left, g_blinks - 1, cache) + countSplits(right, g_blinks - 1, cache);
                cache[g_blinks].emplace(stone, result);

                return result;
            }

            // step 3
            {
                int64_t result = countSplits(stone * 2024, g_blinks - 1, cache);
                cache[g_blinks].emplace(stone, result);

                return result;
            }
        }

        void v1() const override {
            int goal = 25;
            std::vector<std::map<int64_t, int64_t>> cache(goal + 1);
            int64_t total = 0;
            for (auto s : stones) {
                total += countSplits(s, goal, cache);
            }

            reportSolution(total);
        }

        void v2() const override {
            // don't do that, kills your computer.

            int goal = 75;
            std::vector<std::map<int64_t, int64_t>> cache(goal + 1);
            int64_t total = 0;
            for (auto s : stones) {
                total += countSplits(s, goal, cache);
            }

            reportSolution(total);
        }

        void parseBenchReset() override {
            stones.clear();
        }

        private:
        std::vector<int> stones;
    };

} // namespace

#undef DAY