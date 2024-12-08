#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 8

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string line;
            while (std::getline(input, line)) {
                grid.emplace_back(line.begin(), line.end());
            }
            for (size_t i = 0; i < grid.size(); ++i) {
                for (size_t j = 0; j < grid[i].size(); ++j) {
                    char c = grid[i][j];
                    if (c != '.') {
                        antenna_pairs[c].emplace_back(i, j);
                    }
                }
            }
        }

        // ignoring inner antinodes as the puzzles don't mention them. it's sus though!
        // e.g. what if a pair is 6,6 apart: at 2,2 there is sqrt(8) and sqrt(32) distances, which is 2/3 of one 1/3 of the other.
        // because the thing says "for any pair there are two, on either side of them" ?
        void v1() const override {
            std::set<std::pair<int,int>> resonances;
            for (auto& [k, v] : antenna_pairs) {
                for (size_t i = 0; i < v.size(); ++i) {
                    for (size_t j = i; j < v.size(); ++j) {
                        if (i == j) continue;

                        // for each pair, register the antinode coords. Do not care aobut bounds, prune on this later.
                        auto [b, a] = v[i];
                        auto [y, x] = v[j];

                        const int xdiff = std::abs(a - x);
                        const int ydiff = std::abs(b - y);

                        int x1 = a + xdiff;
                        int x2 = x - xdiff;
                        if (x1 == x) {
                            x1 = a - xdiff;
                            x2 = x + xdiff;
                        }
                        int y1 = b + ydiff;
                        int y2 = y - ydiff;
                        if (y1 == y) {
                            y1 = b - ydiff;
                            y2 = y + ydiff;
                        }

                        //std::cout << "pairs " << b << ", " << a << " and " << y << ", " << x << " have antinodes at " << y1 << ", " << x1 << " and " << y2 << ", " << x2 << "\n";
                        resonances.emplace(y1, x1);
                        resonances.emplace(y2, x2);
                    }
                }
            }

            // prune the ones that are  OOB.
            size_t xmax = grid.size();
            size_t ymax = grid[0].size();

            auto in_bounds_resonances = resonances;
            in_bounds_resonances.clear();
            for (auto& [y, x] : resonances) {
                if (y >= 0 && y < ymax) {
                    if (x >= 0 && x < xmax) {
                        in_bounds_resonances.emplace(y, x);
                    }
                }
            }

            reportSolution(in_bounds_resonances.size());
        }

        void v2() const override {
            reportSolution(0);
        }

        void parseBenchReset() override {
            grid.clear();
        }

        private:
        std::vector<std::vector<char>> grid;
        std::map<char, std::vector<std::pair<int, int>>> antenna_pairs;
    };

} // namespace

#undef DAY