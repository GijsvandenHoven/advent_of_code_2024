#pragma once

#include <iostream>
#include <queue>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 10

NAMESPACE_DEF(DAY) {

struct Node { // directed
    Node(int value, int id) :value(value), id(id) {}
    std::vector<int> in;
    std::vector<int> out;
    int value;
    int id;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::vector<std::vector<int>> grid;
        std::string line;
        while (std::getline(input, line)) {
            grid.emplace_back();
            for (auto c : line) {
                grid.back().emplace_back(c - '0');
            }
        }

        // go over the grid, make a graph.
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                int n_id = static_cast<int>(i * grid.size() + j);
                graph.emplace(n_id, Node(grid[i][j], n_id));
            }
        }

        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {

                for (auto [y,x] : std::array<std::pair<int,int>, 4>{ { {i - 1, j}, { i + 1, j }, { i, j-1 }, { i, j+1 } } }) {
                    if (y < 0 || x < 0) continue;
                    if (y >= grid.size() || x >= grid[y].size()) continue;

                    int source_id = static_cast<int>(y * grid.size() + x);
                    int target_id = static_cast<int>(i * grid.size() + j);
                    // compare nodes

                    auto src = graph.find(source_id);
                    auto trg = graph.find(target_id);
                    if (src == graph.end() || trg == graph.end()) throw std::logic_error("ID issue");

                    if (src->second.value == trg->second.value - 1) { // trg is 1 higher than self
                        src->second.out.emplace_back(target_id);
                    }
                    if (src->second.value == trg->second.value + 1) { // trg is 1 lower than self
                        src->second.in.emplace_back(target_id);
                    }
                }
            }
        }

        // for (auto& [k,v] : graph) {
        //     std::cout << "node " << k << ":\n\t";
        //     std::cout << "val: " << v.value << "\n\t";
        //     std::cout << "in: ";
        //     for (auto i : v.in) { std::cout << i << ", "; }
        //     std::cout << "\n\tout: \n\t";
        //     for (auto i : v.out) { std::cout << i << ", "; }
        //     std::cout << "\n";
        // }
    }

    int BFSReachableNine(int from) const {
        std::set<int> reached;
        std::queue<int> work;
        work.push(from);
        int nines = 0;

        while (! work.empty()) {
            int unit = work.front();
            work.pop();

            auto it = graph.find(unit);
            if (it == graph.end()) throw std::logic_error("unknown node");

            if (it->second.value == 9) {
                ++nines;
            }

            for (int id : it->second.out) {
                if (! reached.contains(id)) {
                    reached.emplace(id);
                    work.push(id);
                }
            }
        }

        return nines;
    }

    void v1() const override {
        // DFS find the # of reachable 9s from the 0s.
        int reachableSum = 0;
        for (auto& [k, v] : graph) {
            if (v.value != 0) continue;

            reachableSum += BFSReachableNine(v.id);
        }
        reportSolution(reachableSum);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::unordered_map<int, Node> graph;
};

} // namespace

#undef DAY