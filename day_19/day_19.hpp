#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 19

NAMESPACE_DEF(DAY) {

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

    int64_t memo(const std::string& goal, int pos, std::vector<int64_t>& cache) const {
        // base case 1 : end reached. No cache emplacement here.
        if (pos == goal.size()) {
            return 1;
        }

        // base case 2: check cache
        if (cache[pos] != -1) {
            return cache[pos]; // for each match for each way, this is the # of ways from this pos.
        }

        // recursion: for all matches check possible then add to cache.
        std::vector<int> possible_continues;
        for (auto& t : available_patterns) {
            if (t.canMatch(goal, pos)) {
                possible_continues.emplace_back(t.pattern.size());
            }
        }

        int64_t total = 0;
        for (auto cont : possible_continues) {
            total += memo(goal, pos + cont, cache);
        }

        cache[pos] = total;

        return total;
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
        }
        reportSolution(possible);
    }

    void v2() const override {
        int64_t possible = 0;
        for (const auto& query : desired_patterns) {
            std::vector<int64_t> cache (query.size(), -1);
            assert(cache.size() == query.size());

            auto r = memo(query, 0, cache);
            // std::cout << query << " = " << r << "\n";
            possible += r;
        }
        reportSolution(possible);
    }

    void parseBenchReset() override {
        desired_patterns.clear();
        available_patterns.clear();
    }

    private:
    std::vector<std::string> desired_patterns;
    std::vector<Towel> available_patterns;
};

} // namespace

#undef DAY