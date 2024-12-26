#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 18

NAMESPACE_DEF(DAY) {

struct Coord {
    int X;
    int Y;
};

inline std::ostream& operator<<(std::ostream& os, const Coord& c) {
    os << "Coord { " << c.X << ", " << c.Y << " }";
    return os;
}

inline bool operator<(const Coord& a, const Coord& b) {
    return a.X < b.X || (a.X == b.X && a.Y < b.Y);
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            spots.emplace_back();
            int v = 0;
            int i = 0;
            while (line[i] != ',') {
                v = v * 10 + (line[i] - '0');
                ++i;
            }
            spots.back().X = v;
            v = 0;
            ++i;
            while (i < line.size()) {
                v = v * 10 + (line[i] - '0');
                ++i;
            }
            spots.back().Y = v;
        }

        // place the spots on the grid numbered by their index + 1. This denotes the point in time they will appear.
        for (int i = 0; i < spots.size(); ++i) {
            int time = i + 1;
            auto& c = spots[i];
            grid[c.Y][c.X] = time;
        }

        // printGrid(12);
    }

    void printGrid(int time_limit) const {
        for (auto& line : grid) {
            for (auto& spot : line) {
                if (spot <= time_limit && spot != 0) {
                    std::cout << "#";
                } else {
                    std::cout << ".";
                }
            }
            std::cout << "\n";
        }
    }

    // number of steps given a time amount
    int BFS(int time_limit) const {
        if (grid[0][0] != 0 && grid[0][0] <= time_limit) return -1; // start spot obstructed, unreachable lmao.

        std::queue<std::pair<int, Coord>> work;
        std::set<Coord> seen;
        work.emplace(0, Coord{ 0, 0 });
        seen.emplace(0, 0);

        while (! work.empty()) {
            auto [cost, spot] = work.front();
            work.pop();

            if (spot.X == grid_size-1 && spot.Y == grid_size-1) { // bottom right
                return cost;
            }

            for (auto [X, Y] : std::array<Coord, 4>{{ {spot.X + 1, spot.Y}, {spot.X - 1, spot.Y}, { spot.X, spot.Y - 1 }, { spot.X, spot.Y + 1 } }}) {
                if (X < 0 || X >= grid_size) continue;
                if (Y < 0 || Y >= grid_size) continue;

                if (seen.contains({X, Y})) continue;

                if (grid[Y][X] != 0 && grid[Y][X] <= time_limit) continue;

                // std::cout << X << ", " << Y << " reachable with cost " << cost + 1 << "\n";
                seen.emplace(X, Y);
                work.emplace(cost + 1, Coord{X, Y});
            }
        }

        return -1; // unreachable.
    }

    void v1() const override {
        reportSolution( BFS(1024));
    }

    void v2() const override {
        Coord c{};
        for (int i = 0; i <= spots.size(); ++i) { // less or equal, we want to try any time including zero.
            // std::cout << i << "\n";

            if (BFS(i) == -1) {
                c = spots[i - 1];
                break;
            }
        }

        reportSolution(std::to_string(c.X) + "," + std::to_string(c.Y));
    }

    void parseBenchReset() override {
        spots.clear();
        grid = std::array<std::array<int, grid_size>, grid_size> {};
    }

    private:
    std::vector<Coord> spots;
    static constexpr int grid_size = 71;
    std::array<std::array<int, grid_size>, grid_size> grid {};
};

} // namespace

#undef DAY