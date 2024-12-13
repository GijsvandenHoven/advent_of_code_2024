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

    static int64_t z3(const Game& g) {
        int64_t gx = static_cast<int64_t>(g.Goal.x) + 10'000'000'000'000;
        int64_t gy = static_cast<int64_t>(g.Goal.y) + 10'000'000'000'000;

        std::ofstream smt("d13_z3.txt");

        smt << "(declare-const A Int)\n";
        smt << "(declare-const B Int)\n\n";
        smt << "(assert (and\n(>= A 0) (>= B 0)\n";

        // x
        smt << "(= " << gx << " (+ ( * A " << g.A.x << ") (* B " << g.B.x << ")))\n";
        // y
        smt << "(= " << gy << " (+ ( * A " << g.A.y << ") (* B " << g.B.y << ")))\n";

        smt << "\n))\n\n";

        smt << "(minimize (+ (* 3 A) (* 1 B)))\n\n";
        smt << "(check-sat)\n";
        smt << "(get-objectives)\n";
        smt << "(get-model)";


        smt.close();

        std::array<char, 128> buffer {};
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("z3 -smt2 d13_z3.txt", "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
            result += buffer.data();
        }

        if (result.starts_with("sat")) {
            std::stringstream scan(result);
            std::string line;
            std::getline(scan, line); // 'sat'
            std::getline(scan, line); // '(objectives'
            std::getline(scan, line); // '(answer)'


            // std::cout << line << "\n\t";

            std::stringstream lscan(line);
            for (int i = 0; i < 21; ++i) { lscan.get(); }
            int64_t answer;
            lscan >> answer;

            return answer;
        }

        return 0;
    }

    static int bruteforceWin(const Game& g) {
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
            // std::cout << g << "\n cost: " << c << "\n";
            cost += c;
        }
        reportSolution(cost);
    }

    void v2() const override {
        int64_t cost = 0;
        for (auto& g : games) {
            int64_t c = z3(g);
            cost += c;
        }
        reportSolution(cost);
    }

    void parseBenchReset() override {
        games.clear();
    }

    private:
    std::vector<Game> games;
};

} // namespace

#undef DAY