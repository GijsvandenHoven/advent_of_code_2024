#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 21

NAMESPACE_DEF(DAY) {

struct Code {
    explicit Code(std::string input) {
        if (input.size() != 4) throw std::logic_error("str size");

        for (int i = 0; i < 4; ++i) {
            code[i] = input[i];
        }
    }

    std::array<char, 4> code;

    [[nodiscard]] int numeric_value() const {
        int v = 0;
        for (auto c : code) {
            if (c == 'A') break;
            v *= 10;
            v += (c - '0');
        }

        return v;
    }
};

/*
    +---+---+---+
    | 7 | 8 | 9 |
    +---+---+---+
    | 4 | 5 | 6 |
    +---+---+---+
    | 1 | 2 | 3 |
    +---+---+---+
        | 0 | A |
        +---+---+

        +---+---+
        | ^ | A |
    +---+---+---+
    | < | v | > |
    +---+---+---+
 */

enum class Dir {
    UP = '^',
    DOWN = 'v',
    LEFT = '<',
    RIGHT = '>',
    FWD = 'A'
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;

        while (std::getline(input, line)) {
            codes.emplace_back(line);
        }
    }

    // insanely brain dead code duplication
    static void move_keypad(char from, char to, std::vector<Dir>& out) {
        int srow, scol;
        int erow, ecol;
        switch (from) {
            case 'A':
                srow = 3;
                scol = 2;
                break;
            case '0':
                srow = 3;
                scol = 1;
                break;
            case '1':
                srow = 2;
                scol = 0;
                break;
            case '2':
                srow = 2;
                scol = 1;
                break;
            case '3':
                srow = 2;
                scol = 2;
                break;
            case '4':
                srow = 1;
                scol = 0;
                break;
            case '5':
                srow = 1;
                scol = 1;
                break;
            case '6':
                srow = 1;
                scol = 2;
                break;
            case '7':
                srow = 0;
                scol = 0;
                break;
            case '8':
                srow = 0;
                scol = 1;
                break;
            case '9':
                srow = 0;
                scol = 2;
                break;
            default: throw std::logic_error("unknown char");
        }
        switch (to) {
            case 'A':
                erow = 3;
                ecol = 2;
            break;
            case '0':
                erow = 3;
                ecol = 1;
            break;
            case '1':
                erow = 2;
                ecol = 0;
            break;
            case '2':
                erow = 2;
                ecol = 1;
            break;
            case '3':
                erow = 2;
                ecol = 2;
            break;
            case '4':
                erow = 1;
                ecol = 0;
            break;
            case '5':
                erow = 1;
                ecol = 1;
            break;
            case '6':
                erow = 1;
                ecol = 2;
            break;
            case '7':
                erow = 0;
                ecol = 0;
            break;
            case '8':
                erow = 0;
                ecol = 1;
            break;
            case '9':
                erow = 0;
                ecol = 2;
            break;
            default: throw std::logic_error("unknown char");
        }

        int crow = srow;
        int ccol = scol;

        // heuristics for reducing input after 3 layers of indirection:
        /*
        prefer consecutive over alternating
        left before down
        left before up
        down before right
        */

        // we have a choice if we are not in danger of crossing the gap, i.e. start on bottom row or left column, and end on (other) bottom row, left column.
        if ((srow == 3 && ecol == 0) || (scol == 0 && erow == 3)) {
            // thus we can be naive, prefer up over left to avoid the gap, prefer right over down to avoid the gap. this is because consecutive over alternating.
            while (crow > erow) { // highest prio: go up (get away from the gap)
                out.emplace_back(Dir::UP);
                crow--;
            }
            while (ccol < ecol) { // second prio: go right (get away from the gap)
                out.emplace_back(Dir::RIGHT);
                ccol++;
            }
            while (crow < erow) { // third: go down
                out.emplace_back(Dir::DOWN);
                crow++;
            }
            while (ccol > ecol) { // last: go left
                out.emplace_back(Dir::LEFT);
                ccol--;
            }
        } else { // we have to use the heuristics here.
            while (ccol > ecol) { // left before down or up
                out.emplace_back(Dir::LEFT);
                ccol--;
            }
            while (crow < erow) { // down before right
                out.emplace_back(Dir::DOWN);
                crow++;
            }
            while (crow > erow) { // neutral placement between right and up...
                out.emplace_back(Dir::UP);
                crow--;
            }
            while (ccol < ecol) { // neutral placement between right and up...
                out.emplace_back(Dir::RIGHT);
                ccol++;
            }
        }

        out.emplace_back(Dir::FWD);
    }

    static void move_directional(Dir from, Dir to, std::vector<Dir>& out) {
        int srow, scol;
        int erow, ecol;
        switch(from) {
            case Dir::FWD:
                srow = 1;
                scol = 2;
                break;
            case Dir::UP:
                srow = 1;
                scol = 1;
                break;
            case Dir::DOWN:
                srow = 0;
                scol = 1;
                break;
            case Dir::LEFT:
                srow = 0;
                scol = 0;
                break;
            case Dir::RIGHT:
                srow = 0;
                scol = 2;
                break;
            default: throw std::logic_error("Unknown Dir");
        }
        switch(to) {
            case Dir::FWD:
                erow = 1;
                ecol = 2;
                break;
            case Dir::UP:
                erow = 1;
                ecol = 1;
                break;
            case Dir::DOWN:
                erow = 0;
                ecol = 1;
                break;
            case Dir::LEFT:
                erow = 0;
                ecol = 0;
                break;
            case Dir::RIGHT:
                erow = 0;
                ecol = 2;
                break;
            default: throw std::logic_error("Unknown Dir");
        }

        int crow = srow;
        int ccol = scol;

        while (ccol < ecol) { // highest prio: go right (gap)
            out.emplace_back(Dir::RIGHT);
            ccol++;
        }
        while (crow > erow) { // second: go down (gap)
            out.emplace_back(Dir::DOWN);
            crow--;
        }
        while (ccol > ecol) { // third: go left
            out.emplace_back(Dir::LEFT);
            ccol--;
        }
        while (crow < erow) { // last: up
            out.emplace_back(Dir::UP);
            crow++;
        }

        out.emplace_back(Dir::FWD);
    }

    static void generate_step(const std::vector<Dir>& from, std::vector<Dir>& to) {
        auto current_dir = Dir::FWD;
        for (auto d : from) {
            move_directional(current_dir, d, to);
            current_dir = d;
        }
    }

    void v1() const override {
        int64_t complexity = 0;
        for (auto& code : codes) {
            std::vector<Dir> final_seq; // this is the directional pad input to drive the numeric pad

            // keypad to first bot
            auto current_char = 'A';
            for (auto c : code.code) {
                move_keypad(current_char, c, final_seq);
                current_char = c;
            }

            std::vector<Dir> current = std::move(final_seq);
            std::vector<Dir> next;
            for (int i = 0; i < 2; ++i) {
                generate_step(current, next);
                current = std::move(next);
                next.clear(); // i dont think this is necessary? im not sure about move semantics and husk objects anymore.
            }

            int64_t this_one = (static_cast<int64_t>(current.size()) * code.numeric_value());
            complexity += this_one;
        }

        reportSolution(complexity);
    }

    void v2() const override {


        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<Code> codes;
};

} // namespace

#undef DAY