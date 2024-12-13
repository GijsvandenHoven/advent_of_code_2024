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

            games.push_back(g);
            // std::cout << g << "\n";
        }
    }

    int bruteforceWin(const Game& g) const {
        for (int a = 0; a <= 100; ++a) {
            int ngx = g.Goal.x - (a * g.A.x);
            int ngy = g.Goal.y - (a * g.A.y);

            for (int b = 0; b <= 100; ++b) {
                int egx = ngx - (b * g.B.x);
                int egy = ngy - (b * g.B.y);

                if (egx == 0 && egy == 0) {
                    return a * 3 + b;
                }
            }
        }

        return 0;
    }

    void v1() const override {
        int cost = 0;
        for (auto& g : games) {
            int c = bruteforceWin(g);
            std::cout << g << "\n cost: " << c << "\n";
            cost += c;
        }
        reportSolution(cost);
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {

    }

    private:
    std::vector<Game> games;
};

} // namespace

#undef DAY