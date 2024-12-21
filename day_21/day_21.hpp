#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 21

#define I_HAVE_100GB_RAM false

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

enum class Dir : uint8_t {
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
            while (crow > erow) { // up is actually better than right unless there are 3 or 5 steps. But fortunately, the problem is 2 or 20!
                out.emplace_back(Dir::UP);
                crow--;
            }
            while (ccol < ecol) { // lowest pref.
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

        /*
        prefer consecutive over alternating

        left before down
        left before up
        down before right
        */

        if ((srow == 1 && ecol == 0) || (scol == 0 && erow == 1)) {
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
        } else {
            while (ccol > ecol) { // left before down
                out.emplace_back(Dir::LEFT);
                ccol--;
            }
            while (crow > erow) { // down before right
                out.emplace_back(Dir::DOWN);
                crow--;
            }
            while (crow < erow) { // beware, up and right are not actually ordered like this for depth 3 or depth 5.
                out.emplace_back(Dir::UP);
                crow++;
            }
            while (ccol < ecol) { // lowest prio
                out.emplace_back(Dir::RIGHT);
                ccol++;
            }
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

    // this really seems to under-report slightly, but not until 6 layers deep. I do not know why, i have checked it many times.
    // I instead bruteforced the problem, so this code is incorrect and unused!
    static int64_t memo_find_count(Dir from, Dir to, auto& cache, int rem_depth) {
        if (rem_depth == 0) { // just give a straight answer
            switch (from) {
                case Dir::FWD:
                    switch (to) {
                        case Dir::FWD: return 1; // A
                        case Dir::UP: return 2; // <A
                        case Dir::RIGHT: return 2; //vA
                        case Dir::DOWN: return 3; // v<A
                        case Dir::LEFT: return 4; //v<<A
                        default: throw std::logic_error("No!");
                    }
                case Dir::UP:
                    switch (to) {
                        case Dir::FWD: return 2; // >A
                        case Dir::UP: return 1; // A
                        case Dir::RIGHT: return 3; //v>A
                        case Dir::DOWN: return 2; // vA
                        case Dir::LEFT: return 3; //v<A
                        default: throw std::logic_error("No!");
                    }
                case Dir::RIGHT:
                    switch (to) {
                        case Dir::FWD: return 2; // ^A
                        case Dir::UP: return 3; // <^A
                        case Dir::RIGHT: return 1; //A
                        case Dir::DOWN: return 3; // v<A
                        case Dir::LEFT: return 3; //<<A
                        default: throw std::logic_error("No!");
                    }
                case Dir::DOWN:
                    switch (to) {
                        case Dir::FWD: return 3; // >^A
                        case Dir::UP: return 2; // ^A
                        case Dir::RIGHT: return 2; // >A
                        case Dir::DOWN: return 1; // A
                        case Dir::LEFT: return 2; // <A
                        default: throw std::logic_error("No!");
                    }
                case Dir::LEFT:
                    switch (to) {
                        case Dir::FWD: return 4; // >>^A
                        case Dir::UP: return 3; // >^A
                        case Dir::RIGHT: return 3; // >>A
                        case Dir::DOWN: return 2; // >A
                        case Dir::LEFT: return 1; // A
                        default: throw std::logic_error("No!");
                    }
                default: throw std::logic_error("No!");
            }

        }

        auto& cache_map_from = cache[rem_depth - 1]; // depth 1 remaining is row 0, depth 2 is row 1, etc...
        auto iter = cache_map_from.find(from); // on this layer, this direction is known to take ... ?
        if (iter != cache_map_from.end()) { // we know something about this starting pos at this depth...
            auto inner_iter = iter->second.find(to);
            if (inner_iter != iter->second.end()) { // namely we know how much it will take to get here!
                return inner_iter->second;
            }
        }

        auto memo = [&cache, rem_depth](Dir f, Dir t){ return memo_find_count(f, t, cache, rem_depth- 1); };

        // we don't know, so brute force it. Don't even try the heuristic stuff on keypads, too risky. Just call functions like crazy.
        int64_t option_A = -1, option_B = -1;
        switch (from) {
            case Dir::FWD:
                switch (to) {
                    case Dir::FWD: { // A
                        option_A = memo(Dir::FWD, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::UP: { // <A
                        option_A =
                            memo(Dir::FWD, Dir::LEFT) +
                                memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::RIGHT: { // vA
                        option_A =
                            memo(Dir::FWD, Dir::DOWN) +
                                memo(Dir::DOWN, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::LEFT: { // v<<A, we are certain there is no need for <v<A, we prefer consecutive, this heuristic remains.
                        option_A =
                            memo(Dir::FWD, Dir::DOWN) +
                                memo(Dir::DOWN, Dir::LEFT) +
                                    memo(Dir::LEFT, Dir::LEFT) +
                                        memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::DOWN: { // multiplle choices! <vA  or  v<A
                        option_A = memo(Dir::FWD, Dir::LEFT) +
                            memo(Dir::LEFT, Dir::DOWN) +
                                memo(Dir::DOWN, Dir::FWD);

                        option_B = memo(Dir::FWD, Dir::DOWN) +
                            memo(Dir::DOWN, Dir::LEFT) +
                                memo(Dir::LEFT, Dir::FWD);
                        break;
                    }
                    default: throw std::logic_error("No!");
                }
            break;
            case Dir::UP:
                switch(to) {
                    case Dir::FWD: {
                        // >A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::UP: { // A
                        option_A = memo(Dir::FWD, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::RIGHT: { // choice! >vA  or v>A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::DOWN) + memo(Dir::DOWN, Dir::FWD);
                        option_B = memo(Dir::FWD, Dir::DOWN) + memo(Dir::DOWN, Dir::RIGHT) + memo(Dir::RIGHT, Dir::FWD);
                        break;
                    }
                    case Dir::LEFT: { // v<A
                        option_A = memo(Dir::FWD, Dir::DOWN) + memo(Dir::DOWN, Dir::LEFT) + memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::DOWN: { // vA
                        option_A = memo(Dir::FWD, Dir::DOWN) + memo(Dir::DOWN, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    default:
                        throw std::logic_error("No!");
                }
            break;
            case Dir::RIGHT:
                switch(to) {
                    case Dir::FWD: { // ^A
                        option_A = memo(Dir::FWD, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::UP: { // <^A   or   ^<A
                        option_A = memo(Dir::FWD, Dir::LEFT) + memo(Dir::LEFT, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = memo(Dir::FWD, Dir::UP) + memo(Dir::UP, Dir::LEFT) + memo(Dir::LEFT, Dir::FWD);
                        break;
                    }
                    case Dir::RIGHT: { // A
                        option_A = memo(Dir::FWD, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::LEFT: { // <<A
                        option_A = memo(Dir::FWD, Dir::LEFT) + memo(Dir::LEFT, Dir::LEFT) + memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::DOWN: { // <A
                        option_A = memo(Dir::FWD, Dir::LEFT) + memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    default:
                        throw std::logic_error("No!");
                }
            break;
            case Dir::LEFT:
                switch(to) {
                    case Dir::FWD: { // >>^A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::RIGHT) + memo(Dir::RIGHT, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::UP: { // >^A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::RIGHT: { // >>A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::RIGHT) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::LEFT: { // A
                        option_A = memo(Dir::FWD, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::DOWN: { // >A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    default:
                        throw std::logic_error("No!");
                }
            break;
            case Dir::DOWN:
                switch(to) {
                    case Dir::FWD: { // >^A  or  ^>A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = memo(Dir::FWD, Dir::UP) + memo(Dir::UP, Dir::RIGHT) + memo(Dir::RIGHT, Dir::FWD);
                        break;
                    }
                    case Dir::UP: { // ^A
                        option_A = memo(Dir::FWD, Dir::UP) + memo(Dir::UP, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::RIGHT: { // >A
                        option_A = memo(Dir::FWD, Dir::RIGHT) + memo(Dir::RIGHT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::LEFT: { // <A
                        option_A = memo(Dir::FWD, Dir::LEFT) + memo(Dir::LEFT, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    case Dir::DOWN: { // A
                        option_A = memo(Dir::FWD, Dir::FWD);
                        option_B = option_A;
                        break;
                    }
                    default:
                        throw std::logic_error("No!");
                }
            break;
            default: throw std::logic_error("No!");
        }

        if (option_A < 0 || option_B < 0) throw std::logic_error("A or B was not initialized in the monster switch");

        int64_t best = std::min(option_A, option_B);

        cache[rem_depth-1][from][to] = best;

        return best;
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
            for (int i = 0; i < 24; ++i) {
                std::cout << i << "\n";
                std::cout << current.size() << "\n";
                generate_step(current, next);
                current = std::move(next);
                next.clear(); // i dont think this is necessary? im not sure about move semantics and husk objects anymore.
            }

            // we now have one more layer to go, let's not generate it. calculate this.
            std::cout << "Done grinding with " << current.size() << " items\n";


            auto decider = [](Dir from, Dir to) {
                switch (from) {
                    case Dir::FWD:
                        switch (to) {
                            case Dir::FWD: return 1; // A
                            case Dir::UP: return 2; // <A
                            case Dir::RIGHT: return 2; //vA
                            case Dir::DOWN: return 3; // v<A
                            case Dir::LEFT: return 4; //v<<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::UP:
                        switch (to) {
                            case Dir::FWD: return 2; // >A
                            case Dir::UP: return 1; // A
                            case Dir::RIGHT: return 3; //v>A
                            case Dir::DOWN: return 2; // vA
                            case Dir::LEFT: return 3; //v<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::RIGHT:
                        switch (to) {
                            case Dir::FWD: return 2; // ^A
                            case Dir::UP: return 3; // <^A
                            case Dir::RIGHT: return 1; //A
                            case Dir::DOWN: return 3; // v<A
                            case Dir::LEFT: return 3; //<<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::DOWN:
                        switch (to) {
                            case Dir::FWD: return 3; // >^A
                            case Dir::UP: return 2; // ^A
                            case Dir::RIGHT: return 2; // >A
                            case Dir::DOWN: return 1; // A
                            case Dir::LEFT: return 2; // <A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::LEFT:
                        switch (to) {
                            case Dir::FWD: return 4; // >>^A
                            case Dir::UP: return 3; // >^A
                            case Dir::RIGHT: return 3; // >>A
                            case Dir::DOWN: return 2; // >A
                            case Dir::LEFT: return 1; // A
                            default: throw std::logic_error("No!");
                        }
                    default: throw std::logic_error("No!");
                }
            };

            auto from = Dir::FWD;
            int64_t size = 0;
            for (auto to : current) {
                size += decider(from, to);
                from = to;
            }

            auto code_len = size;
            //auto code_len = static_cast<int64_t>(current.size());
            int64_t this_one = code_len * code.numeric_value();

            complexity += this_one;
        }

        reportSolution(complexity);
    }

    // you will need 96 GB memory and 1 hour of time for this. This is truly a terrible solution.
    void v2() const override {
        int64_t complexity = 0;
#if I_HAVE_100GB_RAM // careful, you could really fry your computer with this. get enough swap space, should take about an hour.
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
            for (int i = 0; i < 24; ++i) {
                std::cout << i << "\n";
                std::cout << current.size() << "\n";
                generate_step(current, next);
                current = std::move(next);
                next.clear(); // i dont think this is necessary? im not sure about move semantics and husk objects anymore.
            }

            // we now have one more layer to go, let's not generate it. calculate this.
            std::cout << "Done grinding with " << current.size() << " items\n";


            auto decider = [](Dir from, Dir to) {
                switch (from) {
                    case Dir::FWD:
                        switch (to) {
                            case Dir::FWD: return 1; // A
                            case Dir::UP: return 2; // <A
                            case Dir::RIGHT: return 2; //vA
                            case Dir::DOWN: return 3; // v<A
                            case Dir::LEFT: return 4; //v<<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::UP:
                        switch (to) {
                            case Dir::FWD: return 2; // >A
                            case Dir::UP: return 1; // A
                            case Dir::RIGHT: return 3; //v>A
                            case Dir::DOWN: return 2; // vA
                            case Dir::LEFT: return 3; //v<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::RIGHT:
                        switch (to) {
                            case Dir::FWD: return 2; // ^A
                            case Dir::UP: return 3; // <^A
                            case Dir::RIGHT: return 1; //A
                            case Dir::DOWN: return 3; // v<A
                            case Dir::LEFT: return 3; //<<A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::DOWN:
                        switch (to) {
                            case Dir::FWD: return 3; // >^A
                            case Dir::UP: return 2; // ^A
                            case Dir::RIGHT: return 2; // >A
                            case Dir::DOWN: return 1; // A
                            case Dir::LEFT: return 2; // <A
                            default: throw std::logic_error("No!");
                        }
                    case Dir::LEFT:
                        switch (to) {
                            case Dir::FWD: return 4; // >>^A
                            case Dir::UP: return 3; // >^A
                            case Dir::RIGHT: return 3; // >>A
                            case Dir::DOWN: return 2; // >A
                            case Dir::LEFT: return 1; // A
                            default: throw std::logic_error("No!");
                        }
                    default: throw std::logic_error("No!");
                }
            };

            auto from = Dir::FWD;
            int64_t size = 0;
            for (auto to : current) {
                size += decider(from, to);
                from = to;
            }

            auto code_len = size;
            //auto code_len = static_cast<int64_t>(current.size());
            int64_t this_one = code_len * code.numeric_value();

            complexity += this_one;
        }
#endif
        reportSolution(complexity);
    }

    // this is too low:
    // 426647904252

    // too low!
    // 304299852534422

    // try 307055584161760 - YES

    void parseBenchReset() override {
        codes.clear();
    }

    private:
    std::vector<Code> codes;
};

} // namespace

#undef DAY