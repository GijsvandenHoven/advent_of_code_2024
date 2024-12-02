#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 2

NAMESPACE_DEF(DAY) {

enum class Delta {
    UP, DOWN, ZERO
};

struct Report {
    explicit Report(const std::string& input) {
        std::stringstream ss(input);
        while (! ss.eof()) {
            int i;
            ss >> i;
            data.emplace_back(i);
        }

        // figure out the delta direction of this report by majority vote.
        std::array<int,3> buckets = {0,0,0};
        for (int i = 1; i < data.size(); ++i) {
            int delta = getDelta(i-1, i);
            buckets[delta+1]++; // map delta to index [0,2]
        }
        int majorityIndex = 0;
        int majorityCount = 0;
        for (int i = 0; i < 3; ++i) {
            if (buckets[i] > majorityCount) {
                majorityCount = buckets[i];
                majorityIndex = i;
            }
        }

        majorityDelta = deltaToEnum(majorityIndex - 1); // map back to [-1,1]
    }

    [[nodiscard]] bool deltaInLimit(int lower, int higher, int i, int j) const {
        int delta = std::abs(data[i] - data[j]);
        return delta >= lower && delta <= higher;
    }

    [[nodiscard]] int getDelta(int i, int j) const {
        int delta = data[i] - data[j];

        return std::clamp(delta, -1, 1);
    }

    [[nodiscard]] static Delta deltaToEnum(int i) {
        switch (i) {
            case -1:
                return Delta::DOWN;
            case 0:
                return Delta::ZERO;
            case 1:
                return Delta::UP;
            default: throw std::logic_error("OOB Index");
        }
    }

    [[nodiscard]] bool monotonic(int i, int j, Delta expect) const {
        return expect == deltaToEnum(getDelta(i, j));
    }

    [[nodiscard]] bool reportOK(int tolerance, std::pair<int,int> deltaTolerance) const {

        auto& [deltaMin, deltaMax] = deltaTolerance;

        // lambda for checking if a pair of entries is OK.
        auto ok = [&](int i, int next){ return deltaInLimit(deltaMin, deltaMax, i, next) && monotonic(i, next, majorityDelta); };

        // lambda for checking _all_ entries. This must be a lambda, because an edge case requires running multiple times: (first one not ok).
        auto solver = [&ok, this](int i, int next, int tolerance) mutable {
            while (static_cast<size_t>(next) < data.size()) {
                if (ok(i, next)) {
                    i = next;
                    ++next;
                } else { // this measurement is not ok, how about we skip the next value?
                    --tolerance;
                    ++next;
                }

                if (tolerance < 0) {
                    return false;
                }
            }

            return true;
        };

        if (! ok(0, 1)) { // the first entry is not ok! gasp! This edge case sucks!! Which one are we gonna drop?? Just try both!
            --tolerance;
            return solver(0, 2, tolerance) || solver(1, 2, tolerance);
        }

        return solver(1, 2, tolerance);
    }

    std::vector<int> data;
    Delta majorityDelta;
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
            assert(reports.back().data.size() >= 2);
        }
    }

    void v1() const override {
        int result = std::accumulate(reports.begin(), reports.end(), 0, [this](int sum, const Report& report) {
            return sum + report.reportOK(0, DELTA_TOLERANCE);
        });
        reportSolution(result);
    }

    void v2() const override {
        int result = std::accumulate(reports.begin(), reports.end(), 0, [this](int sum, const Report& report) {
            return sum + report.reportOK(1, DELTA_TOLERANCE);
        });
        reportSolution(result);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<Report> reports;
    std::pair<int,int> DELTA_TOLERANCE = std::make_pair(1, 3);
};

} // namespace

#undef DAY