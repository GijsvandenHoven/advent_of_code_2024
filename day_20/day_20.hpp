#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 20

NAMESPACE_DEF(DAY) {

enum class TileType : uint8_t {
    WALL,
    EMPTY
};

struct Tile {
    TileType type;
    char label;
    int time = -1;
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            grid.emplace_back();
            for (auto c : line) {
                switch (c) {
                    case 'S':
                    case 'E':
                    case '.':
                        grid.back().emplace_back(TileType::EMPTY, c);
                    break;
                    case '#':
                        grid.back().emplace_back(TileType::WALL, c);
                    break;
                    default: throw std::logic_error("Unknown char");
                }
            }
        }

        // mark the grid with ints
        int sy = -1, sx = -1;
        for (int i = 0; i < grid.size(); ++i) for (int j = 0; j < grid[i].size(); ++j) if (grid[i][j].label == 'S') {
            sy = i;
            sx = j;
        }

        if (sx < 0 || sy < 0) throw std::logic_error("could not find S label");

        std::function<void(int,int,int)> work = [this, &work](int y, int x, int c) {
            if (grid[y][x].type == TileType::WALL) return;
            if (grid[y][x].time >= 0) return; // already filled in.

            grid[y][x].time = c;
            do_to_neighbours(y, x, c+1, work);
        };

        work(sy, sx, 0);
    }

    void do_to_neighbours(int y, int x, int c, std::function<void(int, int, int)>& f) const {
        for (auto [yy, xx] : std::array<std::pair<int,int>, 4> {{{y, x-1}, {y, x+1}, {y-1,x},{y+1,x}}}) {
            if (yy < 0 || yy >= grid.size()) continue;
            if (xx < 0 || xx >= grid[yy].size()) continue;

            f(yy, xx, c);
        }
    }

    void print_grid() const {
        for (auto& line : grid) {
            for (auto& c : line) {
                std::cout << c.label;
            }
            std::cout << "\n";
        }
    }


    void v1() const override {
        constexpr int CHEAT_MIN_VALUE = 100;
        int value_cheat_count = 0;

        std::function cheat_second = [&](int y, int x, int v) {
            int value = (grid[y][x].time - v) - 2; // 2 for the time it took to go through the wall.

            if (value >= CHEAT_MIN_VALUE) {
                std::cout << "\tcheat of value " << value << " at " << y << ", " << x << " from " << v << "\n";
                value_cheat_count += 1;
            }
        };

        std::function cheat_first = [&](int y, int x, int v) { // cheat through a wall.
            if (grid[y][x].type != TileType::WALL) return;

            std::cout << "cheat check on wall at " << y << ", " << x << "\n";

            do_to_neighbours(y, x, v, cheat_second);
        };

        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (grid[i][j].type != TileType::WALL) {
                    // evaluate cheats in this coord.
                    do_to_neighbours(i, j, grid[i][j].time, cheat_first);
                }
            }
        }

        reportSolution(value_cheat_count);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<std::vector<Tile>> grid;
};

} // namespace

#undef DAY