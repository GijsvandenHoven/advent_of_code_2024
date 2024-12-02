#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 2

NAMESPACE_DEF(DAY) {

struct Report {
    explicit Report(const std::string& input) {
        std::stringstream ss(input);
        while (! ss.eof()) {
            int i;
            ss >> i;
            data.emplace_back(i);
        }
    }

    std::vector<int> data;
};

inline std::ostream& operator<< (std::ostream& os, const Report& r) {
    os << "Report  {\n\t";
    for (auto& rr : r.data) {
        os << rr << ", ";
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
            reports.emplace_back(line);
        }

        std::cout << reports.back() << "\n";
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
    std::vector<Report> reports;
};

} // namespace

#undef DAY