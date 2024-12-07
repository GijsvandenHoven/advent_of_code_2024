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

    std::vector<int64_t> values;
    int64_t goal;
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

    static int64_t concat(int64_t left, int64_t right) {
        while (right != 0) {
            const auto digit = right % 10;
            right /= 10;
            left *= 10;
            left += digit;
        }

        return left;
    }

    static bool reachable(int64_t goal, int64_t partial, int idx, const std::vector<int64_t>& values, bool problem_2_enabled) {
        if (partial == goal) return true; // base case 1: reached
        if (partial > goal) return false; // base case 2: past the goal, cannot go down.
        if (idx >= values.size()) return false; // base case 3: not reached but out of values

        auto value = values[idx];
        bool success = false;
        // try add
        success = reachable(goal, partial + value, idx + 1, values, problem_2_enabled);
        if (success) return success;
        // try mul
        success = reachable(goal, partial * value, idx + 1, values, problem_2_enabled);
        if (success) return success;

        // problem 2: concat
        if (problem_2_enabled) {
            success = reachable(goal, concat(partial, value), idx + 1, values, problem_2_enabled);
        }

        return success;
    }

    void v1() const override {
        int64_t can = 0;
        for (auto& p : problems) {
            if(reachable(p.goal, 0, 0, p.values, false)) {
                can += p.goal;
            }
        }
        reportSolution(can);
    }

    // 32277743467738   - too high.
    void v2() const override {
        int64_t can = 0;
        for (auto& p : problems) {
            if(reachable(p.goal, 0, 0, p.values, true)) {
                can += p.goal;
            }
        }
        reportSolution(can);
    }

    void parseBenchReset() override {
        problems.clear();
    }

    private:
    std::vector<Problem> problems;
};

} // namespace

#undef DAY