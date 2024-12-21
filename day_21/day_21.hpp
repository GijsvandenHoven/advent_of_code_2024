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

    void v1() const override {
        int64_t complexity = 0;
        for (auto& code : codes) {
            std::vector<Dir> final_seq; // this is the directional pad input to drive the numeric pad

            auto current_char = 'A';
            for (auto c : code.code) {
                move_keypad(current_char, c, final_seq);
                current_char = c;
            }

            for (auto& d : final_seq) {
                std::cout << static_cast<char>(d);
            }
            std::cout << "\n";

            std::vector<Dir> second; // this is input to dir pad 2, drives dir pad 1.
            auto current_dir = Dir::FWD;
            for (auto d : final_seq) {
                move_directional(current_dir, d, second);
                current_dir = d;
            }

            for (auto& d : second) {
                std::cout << static_cast<char>(d);
            }
            std::cout << "\n";

            std::vector<Dir> third; // this is input to the 3rd dir pad, we are controlling this.
            current_dir = Dir::FWD;
            for (auto d : second) {
                move_directional(current_dir, d, third);
                current_dir = d;
            }

            for (auto& d : third) {
                std::cout << static_cast<char>(d);
            }
            std::cout << "\n";

            int64_t this_one = (static_cast<int64_t>(third.size()) * code.numeric_value());
            std::cout << "code length " << third.size() << "\n";
            std::cout << code.code[0] << code.code[1] << code.code[2] << code.code[3] << ": " << this_one << "\n";
            complexity += this_one;
        }

        // std::vector<Dir> fourth;
        // current_dir = Dir::FWD;
        // for (auto d : third) {
        //     move_directional(current_dir, d, fourth);
        //     current_dir = d;
        // }
        //
        // for (auto& d : fourth) {
        //     std::cout << static_cast<char>(d);
        // }
        // std::cout << "\n";

        reportSolution(complexity);
    }

    void v2() const override {
        // problem statement:

        // <AAv<AA>>^A
        // and
        // v<<AA>^AA>A

        // are both equivalent to '37'
        // namely, they follow from
        // ^^<<A
        // or
        // <<^^A
        // but as demonstrated below, one has a shorter sequence to execute.
        // why?

        // auto _ = [](char c) { return static_cast<Dir>(c); };
        // {
        //     std::vector myInput = {
        //         _('<'),
        //         _('A'),
        //         _('A'),
        //         _('v'),
        //         _('<'),
        //         _('A'),
        //         _('A'),
        //         _('>'),
        //         _('>'),
        //         _('^'),
        //         _('A'),
        //     };
        //
        //     std::vector alternative = {
        //         _('v'),
        //         _('<'),
        //         _('<'),
        //         _('A'),
        //         _('A'),
        //         _('>'),
        //         _('^'),
        //         _('A'),
        //         _('A'),
        //         _('>'),
        //         _('A'),
        //     };
        //
        //     assert(myInput.size() == alternative.size());
        //
        //     std::cout << "\n\n\n\n\n\n";
        //     std::vector<Dir> thing;
        //     auto current = Dir::FWD;
        //     for (auto& d : myInput) {
        //         move_directional(current, d, thing);
        //         current = d;
        //     }
        //
        //     std::cout << "generated input sequence:\n";
        //     for (auto& c : thing) {
        //         std::cout << static_cast<char>(c);
        //     }
        //     std::cout << "\n";
        //     thing.clear();
        //     current = Dir::FWD;
        //     for (auto& d : alternative) {
        //         move_directional(current, d, thing);
        //         current = d;
        //     }
        //     std::cout << "alternative input sequence:\n";
        //     for (auto& c : thing) {
        //         std::cout << static_cast<char>(c);
        //     }
        //     std::cout << "\n";
        //
        //     std::cout << "\n";
        // }
        //
        // // verification of <<^^A and ^^<<A
        // {
        //     std::vector myInput = {
        //         _('^'),
        //         _('^'),
        //         _('<'),
        //         _('<'),
        //         _('A'),
        //     };
        //     std::vector alternative = {
        //         _('<'),
        //         _('<'),
        //         _('^'),
        //         _('^'),
        //         _('A'),
        //     };
        //
        //     std::vector<Dir> thing;
        //     auto current = Dir::FWD;
        //     for (auto& d : myInput) {
        //         move_directional(current, d, thing);
        //         current = d;
        //     }
        //
        //     std::cout << "generated input sequence:\n";
        //     for (auto& c : thing) {
        //         std::cout << static_cast<char>(c);
        //     }
        //     std::cout << "\n";
        //     thing.clear();
        //     current = Dir::FWD;
        //     for (auto& d : alternative) {
        //         move_directional(current, d, thing);
        //         current = d;
        //     }
        //     std::cout << "alternative input sequence:\n";
        //     for (auto& c : thing) {
        //         std::cout << static_cast<char>(c);
        //     }
        //     std::cout << "\n";
        // }


        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<Code> codes;
};

} // namespace

#undef DAY