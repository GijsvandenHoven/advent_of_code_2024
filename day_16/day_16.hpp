#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 16

NAMESPACE_DEF(DAY) {

struct Edge;

struct Node {
    std::vector<Edge> out;

    std::pair<int,int> coords;
    char facing = '?'; // used for debugging only.
};

struct Edge {
    std::shared_ptr<Node> to;
    int cost;
};

inline std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << "Node {\n\t";
    os << "coords: y: " << n.coords.first << ", x: " << n.coords.second << " f: " << n.facing << "\n\t";
    os << "out: {\n";
    for (auto& e : n.out) {
        os << "\t\t{ c: " << e.cost << " (y,x): " << e.to->coords.first << ", " << e.to->coords.second << "}\n";
    }
    os << "\t}\n}";
    return os;
}

struct PathComparator {
    bool operator()(const std::pair<int, const Node*>& a, const std::pair<int, const Node*>& b) const {
        return a.first > b.first;
    }
};

struct PathEntry {
    std::vector<const Node*> parents;
    int cost = 999999999;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override { // yay graph problem
        std::map<std::pair<int,int>, Node> cloud;

        std::string line;
        int row = 0;
        while (std::getline(input, line)) {
            for (size_t i = 0; i < line.size(); ++i) {
                std::pair<int,int> pair = {row, i}; // y, x
                switch(line[i]) {
                    case 'E': {
                        Node n;
                        n.coords = pair;
                        end = pair;
                        cloud[pair] = n;
                        break;
                    }
                    case 'S': {
                        Node n;
                        n.coords = pair;
                        start = pair;
                        cloud[pair] = n;
                        break;
                    }
                    case '.': {
                        Node n;
                        n.coords = pair;
                        cloud[pair] = n;
                    }
                    case '#':
                    default: break;
                }
            }

            ++row;
        }

        char facing[4] = {'N', 'S', 'E', 'W'};
        for (const auto& [k,v] : cloud) {
            for (int i = 0; i < graph.size(); ++i) {
                auto& layer = graph[i];
                layer[k] = std::make_shared<Node>(v); // make a shared ptr copy of this node for each facing direction layer of the graph.
                layer[k]->facing = facing[i];
            }
        }

        // the nodes exist, now connect them using the map keys. Model rotations by representing the graph 4x, each layer being a facing direction!
        // create all the facing edges.
        for (const auto& [k, v] : cloud) {
            // north
            {
                auto iter = graph[0].find({k.first-1, k.second});
                if (iter != graph[0].end()) {
                    graph[0].find(k)->second->out.emplace_back(iter->second, 1);
                }
            }
            // south
            {
                auto iter = graph[1].find({k.first+1, k.second});
                if (iter != graph[1].end()) {
                    graph[1].find(k)->second->out.emplace_back(iter->second, 1);
                }
            }
            // east
            {
                auto iter = graph[2].find({k.first, k.second+1});
                if (iter != graph[2].end()) {
                    graph[2].find(k)->second->out.emplace_back(iter->second, 1);
                }
            }
            // west
            {
                auto iter = graph[3].find({k.first, k.second-1});
                if (iter != graph[3].end()) {
                    graph[3].find(k)->second->out.emplace_back(iter->second, 1);
                }
            }
        }

        // create all the rotational edges.
        for (const auto& [k, v] : cloud) {
            auto north_node = graph[0].find(k)->second;
            auto south_node = graph[1].find(k)->second;
            auto east_node = graph[2].find(k)->second;
            auto west_node = graph[3].find(k)->second;

            // north: rotates east and west.
            north_node->out.emplace_back(east_node, 1000);
            north_node->out.emplace_back(west_node, 1000);

            // south: rotates east and west.
            south_node->out.emplace_back(east_node, 1000);
            south_node->out.emplace_back(west_node, 1000);

            // east: rotates north and south.
            east_node->out.emplace_back(north_node, 1000);
            east_node->out.emplace_back(south_node, 1000);

            // west: rotates north and south.
            west_node->out.emplace_back(north_node, 1000);
            west_node->out.emplace_back(south_node, 1000);
        }
    }

    void print_graph() const {
        // let's try printing this graph.
        for (auto& layer : graph) {
            std::cout << "LAYER: " << "\n";
            for (auto& [k, v] : layer) {
                std::cout << k.first << ", " << k.second << "\n";
                std::cout << (*v) << "\n";
            }
        }
    }

    int dijkstra(const std::pair<int,int>& start_coord, const std::pair<int,int>& end_coord, std::map<const Node*, PathEntry>& path) const {
        // always start facing east.
        // reference north, south, east, west, so index 2.
        auto iter = graph[2].find(start_coord);
        if (iter == graph[2].end()) throw std::logic_error("no start");
        const auto& start_node = iter->second;

        std::map<Node *, int> costs;
        for (auto& layer : graph) {
            for (auto& [k, v] : layer) {
                costs[v.get()] = 999'999'999;
            }
        }
        costs[start_node.get()] = 0;

        std::priority_queue<std::pair<int, const Node*>, std::vector<std::pair<int, const Node*>>, PathComparator> queue;
        queue.emplace(0, start_node.get());

        while (! queue.empty()) {
            auto [cost, node] = queue.top();
            queue.pop();

            for (auto& edge : node->out) {
                auto* neighbour_node = edge.to.get();
                auto node_iter = costs.find(neighbour_node);
                if (node_iter == costs.end()) throw std::logic_error("Cannot find a node");
                int neighbour_cost = node_iter->second;
                int contend_cost = cost + edge.cost;

                if (contend_cost < neighbour_cost) {
                    // when we relax an edge, also update the parent.

                    node_iter->second = contend_cost;
                    queue.emplace(node_iter->second, node_iter->first);
                }

                if (contend_cost <= neighbour_cost) {
                    auto path_iter = path.find(node_iter->first);
                    if (path_iter == path.end()) {
                        PathEntry p;
                        p.cost = contend_cost;
                        p.parents.emplace_back(node);
                        path[node_iter->first] = p;
                        //std::cout << "emplace new " << contend_cost << ", " << node_iter->first->coords.first << ", " << node_iter->first->coords.second << "\n";
                    } else {
                        if (path_iter->second.cost == contend_cost) { // this edge is equal so push it.
                            path_iter->second.parents.emplace_back(node);
                            //std::cout << "add " << contend_cost << "\n";
                        } else { // this edge is better so usurp the others.
                            path_iter->second.cost = contend_cost;
                            path_iter->second.parents.clear();
                            path_iter->second.parents.emplace_back(node);
                            //std::cout << "usurp " << contend_cost << "\n";
                        }
                    }
                }
            }
        }

        int best_cost = 999999999;
        for (auto& layer : graph) {
            auto endNode = layer.find(end)->second;
            best_cost = std::min(best_cost, costs.find(endNode.get())->second);
        }

        return best_cost;
    }

    static void backtrack_best(std::set<const Node*>& p, const PathEntry& here, const Node* goal, const std::map<const Node*, PathEntry>& path) {
        for (const auto* parent : here.parents) {
            // std::cout << parent->coords.first << ", " << parent->coords.second << "\n";
            if (p.contains(parent)) continue;
            p.emplace(parent);
            if (parent == goal) {
                // std::cout << "goalreached\n";
                return;
            }

            backtrack_best(p, path.find(parent)->second, goal, path);
        }
    }

    void v1() const override {
        std::map<const Node*, PathEntry> dontcare; // problem 2 thing.
        reportSolution(dijkstra(start, end, dontcare));
    }

    void v2() const override {
        std::map<const Node*, PathEntry> backpath;
        int best_cost = dijkstra(start, end, backpath);

        std::set<const Node*> on_best;
        for (auto& layer : graph) {
            auto* endptr = layer.find(end)->second.get();
            auto end_iter = backpath.find(endptr);
            if (end_iter == backpath.end()) continue;
            if (end_iter->second.cost > best_cost) continue;

            // std::cout << "found one\n";

            backtrack_best(on_best, end_iter->second, graph[2].find(start)->second.get(), backpath);
            on_best.emplace(endptr);
        }


        // std::cout << on_best.size() << "\n"; // we still need to collapse the graph back to coords...
        std::set<std::pair<int,int>> coords_on_path;

        for (auto * n : on_best) {
            coords_on_path.emplace(n->coords);
        }

        reportSolution(coords_on_path.size());
    }

    void parseBenchReset() override {

    }

    private:
    std::pair<int, int> start;
    std::pair<int, int> end;
    std::array<std::map<std::pair<int,int>, std::shared_ptr<Node>>, 4> graph;
};

} // namespace

#undef DAY