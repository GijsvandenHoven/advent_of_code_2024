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
    BOX = 'O',
    BOX_LEFT = '[',
    BOX_RIGHT = ']'
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
            wide_grid.emplace_back();
            for (auto c : line) {
                grid.back().emplace_back(static_cast<Occupant>(c));

                if (grid.back().back().occupant == Occupant::BOT) {
                    start_bot = {grid.back().size() -1, grid.size() - 1}; // X, Y coords.
                    grid.back().back().occupant = Occupant::EMPTY; // get rid of this mf.
                }

                // also do the wide grid
                switch (const auto conv = static_cast<Occupant>(c)) {
                    case Occupant::WALL:
                    case Occupant::EMPTY:
                        wide_grid.back().emplace_back(conv);
                        wide_grid.back().emplace_back(conv);
                    break;
                    case Occupant::BOX:
                        wide_grid.back().emplace_back(Occupant::BOX_LEFT);
                        wide_grid.back().emplace_back(Occupant::BOX_RIGHT);
                    break;
                    case Occupant::BOT:
                        wide_grid.back().emplace_back(Occupant::EMPTY);
                        start_bot_wide = { wide_grid.back().size() - 1, wide_grid.size() - 1 };
                        wide_grid.back().emplace_back(Occupant::EMPTY);
                    break;
                    default: throw std::logic_error("No");
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

    static bool try_move_big_box(Dir d, Grid& g, std::pair<int,int> boxpos) {
        // boxes are wide, so left/right moves are easier, no double recursion.
        switch (d) {
            case Dir::LEFT: case Dir::RIGHT: {
                auto intended = boxpos;
                Occupant o = g[intended.second][intended.first].occupant;
                while (o == Occupant::BOX_LEFT || o == Occupant::BOX_RIGHT) {
                    // std::cout << " big box horiz move scan... " << intended.first << ", " << intended.second << "\n";
                    intended = map_pos(d, intended);
                    o = g[intended.second][intended.first].occupant;
                }

                switch(o) {
                    case Occupant::EMPTY: { // splendid, the boxes can be moved.
                        Dir rdir = d == Dir::LEFT ? Dir::RIGHT : Dir::LEFT;
                        while (intended != boxpos) {
                            auto next = map_pos(rdir, intended);
                            g[intended.second][intended.first].occupant = g[next.second][next.first].occupant;
                            g[next.second][next.first].occupant = Occupant::EMPTY;
                            intended = next;
                        }
                        return true;
                    }
                    case Occupant::WALL: // do not mutate the grid. We cannot move here.
                        return false;
                    default: throw std::logic_error("Impossible");
                }
            }
            case Dir::DOWN: case Dir::UP: {

                // ok let's check, CAN we move this? This is only if this and the twin's box above (may not be the same) can move!
                // Do not move immediately, let the entire recursion tree collapse first. In case a part of the tree can move but not another part!!
                bool can_move = can_move_big_box_vertical(d, boxpos, g);
                if (can_move) { // great! let's recursively move these things then.
                    move_big_box_vertical(d, boxpos, g);
                }

                return can_move;
            }

            default: throw std::logic_error("Unknown dir type");
        }

        throw std::logic_error("unreachable");
    }

    static void move_big_box_vertical(Dir d, std::pair<int,int> boxpos, Grid& g) {
        auto next_p = map_pos(d, boxpos);

        auto here = g[boxpos.second][boxpos.first].occupant;
        auto next = g[next_p.second][next_p.first].occupant;

        auto twin = boxpos;
        switch (here) {
            case Occupant::BOX_LEFT: // the twin is to the right.
                twin = map_pos(Dir::RIGHT, boxpos);
            break;
            case Occupant::BOX_RIGHT:
                twin = map_pos(Dir::LEFT, boxpos);
            break;
            default: throw std::logic_error("Not an L or R box type");
        }
        auto twin_next_p = map_pos(d, twin);
        auto twin_next_occupant = g[twin_next_p.second][twin_next_p.first].occupant;
        auto twin_occupant = g[twin.second][twin.first].occupant;

        if (next != Occupant::BOX_LEFT &&
            next != Occupant::BOX_RIGHT &&
            twin_next_occupant != Occupant::BOX_LEFT &&
            twin_next_occupant != Occupant::BOX_RIGHT
        ) { // we are done, no more boxes

            assert(next == Occupant::EMPTY); // can_move_big_box_vertical should assure us there are no walls now.

            // swap positions.
            g[next_p.second][next_p.first].occupant = here;
            g[boxpos.second][boxpos.first].occupant = Occupant::EMPTY;

            g[twin_next_p.second][twin_next_p.first].occupant = twin_occupant;
            g[twin.second][twin.first].occupant = Occupant::EMPTY;
        }

        if (here == next) { // great, that means it just moves up, no pyramid type movement.
            move_big_box_vertical(d, next_p, g);
        } else { // uuah
            if (next != Occupant::EMPTY) {
                move_big_box_vertical(d, next_p, g);
            }
            if (twin_next_occupant != Occupant::EMPTY) {
                move_big_box_vertical(d, twin_next_p, g);
            }
        }


        // after all our recursions, we are ready to swap positions ourselves.
        g[next_p.second][next_p.first].occupant = here;
        g[boxpos.second][boxpos.first].occupant = Occupant::EMPTY;
        g[twin_next_p.second][twin_next_p.first].occupant = twin_occupant;
        g[twin.second][twin.first].occupant = Occupant::EMPTY;
    }

    static bool can_move_big_box_vertical(Dir d, std::pair<int,int> boxpos, const Grid& g) {

        if (d != Dir::UP && d != Dir::DOWN) throw std::logic_error("Non vertical in vertical move");

        auto occupant = g[boxpos.second][boxpos.first].occupant;

        if (occupant == Occupant::EMPTY) return true;
        if (occupant == Occupant::WALL) return false;

        // otherwise, there must be more boxes....

        auto twin = boxpos;
        switch (occupant) {
            case Occupant::BOX_LEFT: // the twin is to the right.
                twin = map_pos(Dir::RIGHT, boxpos);
            break;
            case Occupant::BOX_RIGHT:
                twin = map_pos(Dir::LEFT, boxpos);
            break;
            default: throw std::logic_error("Not an L or R box type");
        }

        return can_move_big_box_vertical(d, map_pos(d, boxpos), g) && can_move_big_box_vertical(d, map_pos(d, twin), g);
    }

    static void move_bot(Dir d, Grid& g, std::pair<int,int>& botpos) {
        auto intended_next = map_pos(d, botpos);

        auto go_to = g[intended_next.second][intended_next.first].occupant;
        switch (go_to) {
            case Occupant::EMPTY: // first update pos
                botpos = intended_next;
            case Occupant::WALL: // then we are done.
                return;
            case Occupant::BOT:
                throw std::logic_error("Bot should have been removed from the grid representation");
            case Occupant::BOX: {
                bool result = try_move_box(d, g, intended_next);
                if (result) { // grid was mutated by boxes from try_move_box
                    botpos = intended_next;
                }
                return;
            }
            case Occupant::BOX_LEFT: case Occupant::BOX_RIGHT: {
                bool result = try_move_big_box(d, g, intended_next);
                if (result) {
                    botpos = intended_next;
                }
                return;
            }
            default:
                throw std::logic_error("Unknown type in move bot");
        }
    }

    static int gps_sum(const Grid& g) {
        int gps_sum = 0;
        for (int y = 0; y < g.size(); ++y) {
            for (int x = 0; x < g[y].size(); ++x) {
                if (g[y][x].occupant == Occupant::BOX || g[y][x].occupant == Occupant::BOX_LEFT) {
                    gps_sum += 100 * y + x;
                }
            }
        }
        return gps_sum;
    }

    void v1() const override {
        auto mut_copy = grid;
        auto botpos = start_bot;
        for (auto d : movements) {

            move_bot(d, mut_copy, botpos);
        }

        // get gps coords of boxes.
        reportSolution(gps_sum(mut_copy));
    }

    void v2() const override {
        auto mut_copy = wide_grid;

        auto botpos = start_bot_wide;
        for (auto d : movements) {
            move_bot(d, mut_copy, botpos);
        }

        reportSolution(gps_sum(mut_copy));
    }

    void parseBenchReset() override {
        movements.clear();
        wide_grid.clear();
        grid.clear();
    }

    private:
    std::vector<Dir> movements;
    Grid grid;
    Grid wide_grid;
    std::pair<int,int> start_bot;
    std::pair<int,int> start_bot_wide;
};

} // namespace

#undef DAY