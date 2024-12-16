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
                    Edge e = { iter->second, 1 };
                    graph[0].find(k)->second->out.emplace_back(std::move(e));
                }
            }
            // south
            {
                auto iter = graph[1].find({k.first+1, k.second});
                if (iter != graph[1].end()) {
                    Edge e = { iter->second, 1 };
                    graph[1].find(k)->second->out.emplace_back(std::move(e));
                }
            }
            // east
            {
                auto iter = graph[2].find({k.first, k.second+1});
                if (iter != graph[2].end()) {
                    Edge e = { iter->second, 1 };
                    graph[2].find(k)->second->out.emplace_back(std::move(e));
                }
            }
            // west
            {
                auto iter = graph[3].find({k.first, k.second-1});
                if (iter != graph[3].end()) {
                    // std::cout << "West: " << k.first << ", " << k.second << " to " << iter->first.first << ", " << iter->first.second << "\n";

                    Edge e = { iter->second, 1 };
                    graph[3].find(k)->second->out.emplace_back(std::move(e));
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

    void v1() const override {
        reportSolution(0);
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