#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 1

NAMESPACE_DEF(DAY) {

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while(std::getline(input, line)) {
            std::stringstream ss(line);
            int l, r;
            ss >> l;
            ss >> r;

            left.emplace_back(l);
            right.emplace_back(r);
        }

        assert(left.size() == right.size());
        size = left.size();

        // edit after part 2: both require a sorting of the list, so do it in parse.
        std::ranges::sort(left);
        std::ranges::sort(right);
    }

    void v1() const override {
        int sum = 0;
        for (size_t i = 0; i < size; ++i) {
            sum += (std::abs(left[i] - right[i]));
        }

        reportSolution(sum);
    }

    void v2() const override {
        int right_idx = 0;
        int64_t similarity = std::accumulate(left.begin(), left.end(), 0ll, [&](int64_t sum, int64_t value) {
            while (right_idx < size && right[right_idx] < value) { // scan forward until we hit the (sorted) block of similar numbers
                ++right_idx;
            }
            const auto og = right_idx;
            while (right_idx < size && right[right_idx] == value) { // for each equal in this block, increment the similarity.
                sum += value;
                right_idx++;
            }
            right_idx = og; // restore after scan: the next number on left might also need this block. More elegant would be a cache.
            return sum;
        });
        reportSolution(similarity);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<int> left;
    std::vector<int> right;

    size_t size = 0;
};

} // namespace

#undef DAY