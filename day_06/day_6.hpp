#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 6

NAMESPACE_DEF(DAY) {

class Grid : public std::vector<std::vector<char>> {
public:
    Grid() = default;

    void addLine(const std::string& l) {
        this->emplace_back(l.begin(), l.end());
        size_t x;
        if ((x = l.find_first_of('^')) != std::string::npos) {
            guardPos = { x, this->size() - 1 };
            this->back()[x] = '.';
        }
    }

    std::pair<int, int> guardPos = {-1,-1};
};

inline std::ostream& operator<<(std::ostream& os, const Grid& g) {
    os << "Grid {\n\t";
    for (auto& l : g) {
        for (auto c : l) {
            os << c;
        }
        os << "\n\t";
    }
    os << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;

        while (std::getline(input, line)) {
            g.addLine(line);
        }

        std::cout << g << "\n";
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
    Grid g;
};

} // namespace

#undef DAY