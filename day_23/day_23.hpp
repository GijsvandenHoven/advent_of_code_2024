#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 23

NAMESPACE_DEF(DAY) {

struct Computer {
    std::string id;
    std::set<std::string> connections;

    [[nodiscard]] int degree() const { return static_cast<int>(connections.size()); }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            std::string self = line.substr(0, 2);
            std::string other = line.substr(3, 2);

            computers[self].id = self;
            computers[other].id = other;

            computers[self].connections.emplace(other);
            computers[other].connections.emplace(self);
        }

        for (auto& [k, v] : computers) {
            std::cout << k << ": ";
            for (auto& c : v.connections) {
                std::cout << c << ", ";
            }
            std::cout << "\n";
        }
    }

    void get_triplets(const Computer& src, std::set<std::set<std::string>>& known) const {
        // for each connection of src
        for (auto& con : src.connections) {
            // find another one we have in common. That is a triplet.
            auto& other = computers.find(con)->second;

            for (auto& c2 : other.connections) {
                std::set sorted { src.id, con, c2 };
                if (src.connections.contains(c2) && ! known.contains(sorted)) {
                    known.emplace(sorted);
                }
            }
        }
    }

    void v1() const override {
        std::set<std::set<std::string>> lex_sorted_triplets;
        for (auto& [key, value] : computers) {
            if (key[0] != 't') continue;

            get_triplets(value, lex_sorted_triplets);
        }
        reportSolution(lex_sorted_triplets.size());
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        computers.clear();
    }

    private:
    std::map<std::string, Computer> computers;
};

} // namespace

#undef DAY