#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 13

NAMESPACE_DEF(DAY) {

struct Game {
    struct Coord {
        int x; int y;
    };

    Coord A;
    Coord B;

    Coord Goal;
};

inline std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << "Game {\n\t";
    os << "A: " << g.A.x << ", " << g.A.y << "\n\t";
    os << "B: " << g.B.x << ", " << g.B.y << "\n\t";
    os << "G: " << g.Goal.x << ", " << g.Goal.y << "\n\t";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        while (! input.eof()) {
            Game g {};
            for (int i = 0; i < 2; ++i) {
                // line 1
                while (input.get() != '+') ;
                int x;
                input >> x;
                input.clear();
                while (input.get() != '+') ;
                int y;
                input >> y;
                input.clear();

                if (i == 0) {
                    g.A.x = x;
                    g.A.y = y;
                } else {
                    g.B.x = x;
                    g.B.y =y;
                }
            }
            while (input.get() != '=') ;
            int x;
            input >> x;
            input.clear();
            while (input.get() != '=') ;
            int y;
            input >> y;

            g.Goal.x = x;
            g.Goal.y = y;

            input.clear();
            input.get(); // newline char

            std::cout << g << "\n";
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
};

} // namespace

#undef DAY