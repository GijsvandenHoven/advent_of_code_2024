#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 19

struct Towel {
    std::string pattern;

    [[nodiscard]] bool canMatch(const std::string& matching, int pos) const {
        if (matching.size() - pos < pattern.size()) return false;

        for (auto& c : pattern) {
            if (c != matching[pos]) return false;

            ++pos;
        }

        return true;
    }
};

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string available;
        std::getline(input, available);

        size_t scanner = 0;
        while (true) {
            size_t end = available.find_first_of(',', scanner);
            if (end == std::string::npos) {
                available_patterns.emplace_back(available.substr(scanner));
                break;
            }

            available_patterns.emplace_back(available.substr(scanner, end - scanner));
            scanner = end + 2; // comma and space
        }

        std::getline(input, available);
        while (std::getline(input, available)) {
            desired_patterns.emplace_back(available);
        }
    }

    bool brute_force(const std::string& goal, int pos = 0) const {
        // base case
        // std::cout << "for " << goal << " at " << pos << "\n";
        if (pos == goal.size()) return true;

        // collect all the possible towels from this pos.
        std::vector<int> possible_continues;
        for (auto& t : available_patterns) {
            if (t.canMatch(goal, pos)) {
                possible_continues.emplace_back(t.pattern.size());
            }
        }

        for (auto cont : possible_continues) {
            if (brute_force(goal, pos + cont)) {
                return true;
            }
        }

        return false;
    }

    void v1() const override {
        // theres  a DP here but lets start with brute force.
        int possible = 0;
        for (const auto& query : desired_patterns) {
            possible += brute_force(query);
            std::cout << possible << "\n";
        }
        reportSolution(possible);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<std::string> desired_patterns;
    std::vector<Towel> available_patterns;
};

} // namespace

#undef DAY