#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 18

NAMESPACE_DEF(DAY) {

struct Coord {
    int X;
    int Y;
};

inline std::ostream& operator<<(std::ostream& os, const Coord& c) {
    os << "Coord { " << c.X << ", " << c.Y << " }";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            spots.emplace_back();
            int v = 0;
            int i = 0;
            while (line[i] != ',') {
                v = v * 10 + (line[i] - '0');
                ++i;
            }
            spots.back().X = v;
            v = 0;
            ++i;
            while (i < line.size()) {
                v = v * 10 + (line[i] - '0');
                ++i;
            }
            spots.back().Y = v;
        }
    }

    void v1() const override {

        reportSolution(0);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<Coord> spots;
    std::array<std::array<int, 70>, 70> grid {};
};

} // namespace

#undef DAY