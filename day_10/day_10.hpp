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
    }

    // this is a silly solution, i could have just done the normal BFS and count it anyway when a node == 9. There is no need to first get the 9s then go again per 9.
    int BFSRating(int from, int to) const {
         // how many ways to get from the start to the end?
        std::queue<int> work;
        work.push(from);
        int rating = 0;

        while (! work.empty()) {
            int unit = work.front();
            work.pop();

            auto it = graph.find(unit);
            if (it == graph.end()) throw std::logic_error("unknown node");

            if (it->first == to) {
                ++rating;
            }

            for (int id : it->second.out) {
                work.push(id);
            }
        }

        // std::cout << " rated: " << rating << "\n";
        return rating;
    }

    int BFSReachableNine(int from, std::vector<int>& nines_reachable) const {
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
                nines_reachable.emplace_back(it->first);
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
        std::vector<int> dont_care; // used for p2
        for (auto& [k, v] : graph) {
            if (v.value != 0) continue;

            reachableSum += BFSReachableNine(v.id, dont_care);
            dont_care.clear();
        }
        reportSolution(reachableSum);
    }

    void v2() const override {
        // DFS find the # of reachable 9s from the 0s.
        int ratingSum = 0;
        for (auto& [k, v] : graph) {
            if (v.value != 0) continue;

            std::vector<int> nines; // used forr p2
            BFSReachableNine(v.id, nines);

            // pathfinding DFS on a 0-9 pair to get its rating
            for (auto nine : nines) {
                ratingSum += BFSRating(v.id, nine);
            }
        }
        reportSolution(ratingSum);
    }

    void parseBenchReset() override {
        graph.clear();
    }

    private:
    std::unordered_map<int, Node> graph;
};

} // namespace

#undef DAY