#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 7

NAMESPACE_DEF(DAY) {

struct Problem {

    explicit Problem(const std::string& input) : goal(0) {
        std::stringstream ss (input);

        ss >> goal;
        ss.get(); // colon char

        int x;
        while (ss >> x) {
            values.emplace_back(x);
        }
    }

    std::vector<int> values;
    int goal;
};

inline std::ostream& operator<<(std::ostream& os, const Problem& p) {
    os << "Problem {\n\t";
    os << p.goal << ": ";
    for (const auto v : p.values) { os << v << ", "; }
    os << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            problems.emplace_back(line);
        }
    }

    void v1() const override {
        reportSolution(0);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        problems.clear();
    }

    private:
    std::vector<Problem> problems;
};

} // namespace

#undef DAY