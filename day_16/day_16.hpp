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

        char facing[4] = {'N', 'E', 'S', 'W'};
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

    int dijkstra(const std::pair<int,int>& start_coord, const std::pair<int,int>& end_coord) const {
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
            auto& [cost, node] = queue.top();

            for (auto& edge : node->out) {
                auto* neighbour_node = edge.to.get();
                auto node_iter = costs.find(neighbour_node);
                if (node_iter == costs.end()) throw std::logic_error("Cannot find a node");
                int neighbour_cost = node_iter->second;

                if (cost + edge.cost < neighbour_cost) {
                    std::cout << "Relax cost of " << node_iter->first->coords.first << ", " << node_iter->first->coords.second << " from " << neighbour_cost << " to " << cost + edge.cost << "\n";

                    node_iter->second = cost + edge.cost;
                    queue.emplace(node_iter->second, node_iter->first);
                }
            }

            queue.pop();
        }

        return 0;
    }

    void v1() const override {
        print_graph();

        reportSolution(dijkstra(start, end));
    }

    void v2() const override {
        reportSolution(0);
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