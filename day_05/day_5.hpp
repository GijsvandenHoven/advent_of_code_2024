#pragma once

#include <iostream>
#include <unordered_set>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 5

NAMESPACE_DEF(DAY) {

typedef std::unordered_map<int, std::set<int>> Dependencies;

struct PrintOrder {
    explicit PrintOrder(const std::vector<int>&& need) : need(need) {
        for (auto& n : this->need) {
            updates.emplace(n);
        }
    }

    [[nodiscard]] int middle() const {
        return need[need.size()/2];
    }

    std::vector<int> need;
    std::set<int> updates;
};

inline std::ostream& operator<<(std::ostream& os, const PrintOrder& po) {
    os << "PrintOrder {\n\t";
    for (auto& n : po.need) {
        os << n << ", ";
    }
    os << "\n}";
    return os;
}

struct DAG {
    DAG() : value(-1) {}

    DAG(int v, const std::set<int>& deps, const std::set<int>& scope) {
        value = v;

        // intersect deps and scope.
        for (auto d : deps) {
            if (scope.contains(d)) {
                in.emplace(d);
            }
        }
    }

    int value;
    std::set<int> in;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) { // rules

            if (line.empty()) break;

            // observation: its all 2 digit numbers
            assert(line.size() == 5);
            int left = std::stoi(line.substr(0, 2));
            int right = std::stoi(line.substr(3, 2));

            deps[right].emplace(left);
        }

        while (std::getline(input, line)) { // orders
            // because of the assertion, sensible orders only have 2 digits each also.
            std::vector<int> numbers;
            for (size_t i = 0; i < line.size(); i += 3) {
                numbers.emplace_back(std::stoi(line.substr(i, 2)));
            }
            orders.emplace_back(std::move(numbers));
        }

        for (auto& o : orders) {
            assert(o.need.size() % 2 == 1);
        }
    }

    bool canPrint(const PrintOrder& p) const {
        std::unordered_set<int> printed;

        for (auto& page : p.need) { // for each page that we need, check if it can be printed.
            // a page can be printed if it does not depends on previous pages.
            auto iter = deps.find(page);
            if (iter != deps.end()) {
                const auto& depending = iter->second;
                // no dependencies on this page at all.
                for (auto d : depending) {
                    if (p.updates.contains(d)) { // careful, the page we are printing depends on another that needs to be printed. Was it printed yet?
                        if (! printed.contains(d)) {
                            return false;
                        }
                    }
                }
            }

            // ok, we can print it!
            printed.emplace(page);
        }

        return true;
    }

    void fixPrint(PrintOrder& p) const {
        std::unordered_map<int, DAG> dag;
        for (auto val : p.need) {
            auto iter = deps.find(val);
            if (iter == deps.end()) {
                throw std::logic_error("not found");
            }
            dag[val] = DAG(val, iter->second, p.updates);
        }

        std::set<int> printed;
        p.need.clear();
        while (p.need.size() < p.updates.size()) {
            // find next in DAG
            for (auto& [k, v] : dag) {
                bool can = true;
                if (printed.contains(k)) {
                    continue;
                }
                for (auto& prereq : v.in) {
                    can = can && printed.contains(prereq);
                }
                if (can){
                    p.need.emplace_back(k);
                    printed.emplace(k);
                }
            }
        }
    }

    void v1() const override {
        int r = std::accumulate(orders.begin(), orders.end(), 0, [this](int sum, const PrintOrder& p) {
            bool printable = canPrint(p);
            return sum + (printable ? p.middle() : 0);
        });
        reportSolution(r);
    }

    void v2() const override {
        int sum = 0;
        for (auto& p : orders) {
            if (! canPrint(p)) {
                PrintOrder needFix = p;
                fixPrint(needFix);

                sum += needFix.middle();
            }
        }
        reportSolution(sum);
    }

    void parseBenchReset() override {
        deps.clear();
        orders.clear();
    }

    private:
        Dependencies deps;
        std::vector<PrintOrder> orders;
};

} // namespace

#undef DAY