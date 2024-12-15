#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 15

NAMESPACE_DEF(DAY) {

enum class Dir : uint8_t {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

inline Dir c_to_dir(char c) {
    switch (c) {
        case '^': return Dir::UP;
        case 'v': return Dir::DOWN;
        case '<': return Dir::LEFT;
        case '>': return Dir::RIGHT;
        default: throw std::logic_error("Unknown char for Dir");
    }
}

inline std::ostream& operator<<(std::ostream& os, const Dir& d) {
    switch (d) {
        case Dir::UP:
            os << '^';
        break;
        case Dir::DOWN:
            os << 'v';
        break;
        case Dir::LEFT:
            os << '<';
        break;
        case Dir::RIGHT:
            os << '>';
        break;
    }

    return os;
}

enum class Occupant : char {
    WALL = '#',
    EMPTY = '.',
    BOT = '@',
    BOX = 'O'
};

struct Tile {
    Occupant occupant;
};

using Grid = std::vector<std::vector<Tile>>;

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) break; // the empty line between the maze and the sequence.

            grid.emplace_back();
            for (auto c : line) {
                grid.back().emplace_back(static_cast<Occupant>(c));

                if (grid.back().back().occupant == Occupant::BOT) {
                    start_bot = {grid.back().size() -1, grid.size() - 1}; // X, Y coords.
                    grid.back().back().occupant = Occupant::EMPTY; // get rid of this mf.
                }
            }
        }

        while (std::getline(input, line)) {
            for (auto c : line) {
                movements.emplace_back(c_to_dir(c));
            }
        }
    }

    static std::pair<int,int> map_pos(Dir d, const std::pair<int,int> p) {
        auto n = p;
        switch (d) {
            case Dir::UP:
                n.second -= 1;
            break;
            case Dir::DOWN:
                n.second += 1;
            break;
            case Dir::LEFT:
                n.first -= 1;
            break;
            case Dir::RIGHT:
                n.first += 1;
            break;
        }

        return n;
    }

    static bool try_move_box(Dir d, Grid& g, std::pair<int,int> boxpos) {
        // can we move the box?
        auto intended = map_pos(d, boxpos);

        auto occupant = g[intended.second][intended.first].occupant;
        if (occupant == Occupant::WALL) return false;
        if (occupant == Occupant::EMPTY) {
            // swap box position and return true.
            g[intended.second][intended.first].occupant = Occupant::BOX;
            g[boxpos.second][boxpos.first].occupant = Occupant::EMPTY;

            return true;
        }

        if (occupant == Occupant::BOX) { // recursion.
            bool allowed = try_move_box(d, g, intended);
            if (allowed) {
                // swap box position and return true.
                g[intended.second][intended.first].occupant = Occupant::BOX;
                g[boxpos.second][boxpos.first].occupant = Occupant::EMPTY;
            }
            return allowed;
        }

        throw std::logic_error("shouldn't reach this.");
    }

    static void move_bot(Dir d, Grid& g, std::pair<int,int>& botpos) {
        auto intended_next = map_pos(d, botpos);

        std::cout << "mov " << intended_next.first << ", " << intended_next.second << "\n";

        auto go_to = g[intended_next.second][intended_next.first].occupant;
        switch (go_to) {
            case Occupant::EMPTY: // first update pos
                std::cout << "empty\n";
                botpos = intended_next;
            case Occupant::WALL: // then we are done.
                std::cout << "wall\n";
                return;
            case Occupant::BOT:
                throw std::logic_error("Bot should have been removed from the grid representation");
            case Occupant::BOX:
                std::cout << "box!\n";
                bool result = try_move_box(d, g, intended_next);
                if (result) { // grid was mutated by boxes from try_move_box
                    std::cout << "can push\n";
                    botpos = intended_next;
                }
                return;
        }
    }

    void v1() const override {
        auto mut_copy = grid;
        auto botpos = start_bot;
        // std::cout << botpos.first << ", " << botpos.second << "\n";
        for (auto d : movements) {
            std::cout << "PRE STEP " << d << ": " << botpos.first << ", " << botpos.second << "\n";
            for (auto& l : mut_copy) {
                for (auto [occupant] : l) {
                    std::cout << static_cast<char>(occupant);
                }
                std::cout << "\n";
            }

            move_bot(d, mut_copy, botpos);
        }

        // get gps coords of boxes.
        int gps_sum = 0;
        for (int y = 0; y < mut_copy.size(); ++y) {
            for (int x = 0; x < mut_copy[y].size(); ++x) {
                if (mut_copy[y][x].occupant == Occupant::BOX) {
                    gps_sum += 100 * y + x;
                }
            }
        }

        reportSolution(gps_sum);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        movements.clear();
        grid.clear();
    }

    private:
    std::vector<Dir> movements;
    Grid grid;
    std::pair<int,int> start_bot;
};

} // namespace

#undef DAY