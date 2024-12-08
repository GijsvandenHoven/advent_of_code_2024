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
            // prune the ones that are  OOB.
            size_t xmax = grid.size();
            size_t ymax = grid[0].size();

            for (auto& [k, v] : antenna_pairs) {
                for (size_t i = 0; i < v.size(); ++i) {
                    for (size_t j = i; j < v.size(); ++j) {
                        if (i == j) continue;

                        // for each pair, register the antinode coords.
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

                        if (y1 >= 0 && y1 < ymax) {
                            if (x1 >= 0 && x1 < xmax) {
                                resonances.emplace(y1, x1);
                            }
                        }
                        if (y2 >= 0 && y2 < ymax) {
                            if (x2 >= 0 && x2 < xmax) {
                                resonances.emplace(y2, x2);
                            }
                        }

                        //std::cout << "pairs " << b << ", " << a << " and " << y << ", " << x << " have antinodes at " << y1 << ", " << x1 << " and " << y2 << ", " << x2 << "\n";
                    }
                }
            }

            reportSolution(resonances.size());
        }

        void v2() const override {
            std::set<std::pair<int,int>> resonances;
            // prune the ones that are  OOB.
            size_t xmax = grid.size();
            size_t ymax = grid[0].size();

            auto try_emplace = [&](int y,int x) {
                if (y >= 0 && y < ymax) {
                    if (x >= 0 && x < xmax) {
                        resonances.emplace(y, x);
                        return true;
                    }
                }

                return false;
            };

            for (auto& [k, v] : antenna_pairs) {
                for (size_t i = 0; i < v.size(); ++i) {
                    for (size_t j = i; j < v.size(); ++j) {
                        if (i == j) continue;

                        // for each pair, register the antinode coords.
                        auto [b, a] = v[i];
                        auto [y, x] = v[j];

                        // the antenna itself is an antinode in problem 2.
                        //resonances.emplace(b, a);
                        //resonances.emplace(y, x);

                        const int xdiff = std::abs(a - x);
                        const int ydiff = std::abs(b - y);

                        bool x1plus = true;

                        int x1 = a + xdiff;
                        int x2 = x - xdiff;
                        if (x1 == x) {
                            x1 = a - xdiff;
                            x2 = x + xdiff;
                            x1plus = false;
                        }

                        bool y1plus = true;
                        int y1 = b + ydiff;
                        int y2 = y - ydiff;
                        if (y1 == y) {
                            y1 = b - ydiff;
                            y2 = y + ydiff;
                            y1plus = false;
                        }

                        // x1/y1 emplacmeent loop
                        {
                            int mul = 0;
                            auto coord = [a,x, &mul](int v, int diff, int plus) {
                                int scaled = mul * diff;
                                return plus ? v + scaled : v - scaled;
                            };

                            while (try_emplace(coord(b, ydiff, y1plus), coord(a, xdiff, x1plus))) {
                                ++mul;
                            }
                            mul = 0;
                            while (try_emplace(coord(y, ydiff, !y1plus), coord(x, xdiff, !x1plus))) {
                                ++mul;
                            }
                        }

                        // if (y1 >= 0 && y1 < ymax) {
                        //     if (x1 >= 0 && x1 < xmax) {
                        //         resonances.emplace(y1, x1);
                        //     }
                        // }
                        // if (y2 >= 0 && y2 < ymax) {
                        //     if (x2 >= 0 && x2 < xmax) {
                        //         resonances.emplace(y2, x2);
                        //     }
                        // }

                        //std::cout << "pairs " << b << ", " << a << " and " << y << ", " << x << " have antinodes at " << y1 << ", " << x1 << " and " << y2 << ", " << x2 << "\n";
                    }
                }
            }

            for (size_t i = 0; i < grid.size(); ++i) {
                for (size_t j = 0; j < grid[i].size(); ++j) {
                    auto coord = std::make_pair(j,i);
                    if (resonances.contains(coord)) {
                        std::cout << "#";
                    } else {
                        std::cout << grid[i][j];
                    }
                }
                std::cout << "\n";
            }

            // for (auto [y, x] : resonances) {
            //     std::cout << "at " << y << ", " << x << "\n";
            // }

            reportSolution(resonances.size());
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