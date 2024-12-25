#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 25

NAMESPACE_DEF(DAY) {

constexpr int MAX_HEIGHT = 5;
constexpr int OBJECT_WIDTH = 5;

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void objectToHeight(std::ifstream &input, bool key) {
        auto& target = key ? keys : locks;
        target.emplace_back();

        std::array extending { true, true, true, true, true };
        std::string line;
        for (int h = MAX_HEIGHT; h >= 0; --h) {
            std::getline(input, line);
            for (int i = 0; i < OBJECT_WIDTH; ++i) {
                if (extending[i] && line[i] == (key ? '#' : '.')) { // this one is still extending but we just found a '#'.
                    extending[i] = false;
                    target.back()[i] = key ? h : 5-h;
                }
            }
        }
    }

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) continue;

            objectToHeight(input,  line[0] == '.');
        }
    }

    static bool check_fit(auto& l, auto& k) {

        for (int i = 0; i < OBJECT_WIDTH; ++i) {
            if (l[i] + k[i] > MAX_HEIGHT) return false;
        }

        return true;
    }

    void v1() const override {
        int fits = 0;
        // std::cout << "check " << (locks.size() * keys.size()) << "\n";
        for (auto& lock : locks) {
            for (auto& key : keys) {
                fits += check_fit(lock, key);
            }
        }

        reportSolution(fits);
    }

    void v2() const override { // no part 2 on the last day :) merry christmas
        reportSolution(0);
    }

    void parseBenchReset() override {
        locks.clear();
        keys.clear();
    }

    private:
    std::vector<std::array<int, OBJECT_WIDTH>> locks;
    std::vector<std::array<int, OBJECT_WIDTH>> keys;
};

} // namespace

#undef DAY