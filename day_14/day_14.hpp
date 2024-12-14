#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 14

NAMESPACE_DEF(DAY) {

struct Bot {
    int startX;
    int startY;
    int velX;
    int velY;
};

inline std::ostream& operator<<(std::ostream& os, const Bot& b) {
    os << "Bot {\n\t";
    os << "s: " << b.startX << ", " << b.startY << "\n\tv: " << b.velX << ", " << b.velY << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            Bot b{};
            std::stringstream ss (line);
            ss.get();
            ss.get();
            ss >> b.startX;
            ss.clear();
            ss.get();
            ss >> b.startY;
            ss.clear();
            ss.get();
            ss.get();
            ss.get();
            ss >> b.velX;
            ss.clear();
            ss.get();
            ss >> b.velY;

            // std::cout << b << "!\n";
            bots.emplace_back(b);
        }

        // std::cout << bots.back().startX << ", " << bots.back().startY << ", " << bots.back().velX << ", " << bots.back().velY << "\n";
    }

    void v1() const override {
        std::vector<Bot> result;
        int time = 100;

        for (auto& b : bots) {
            int nx = b.startX + time* b.velX;
            int ny = b.startY + time* b.velY;
            // moodulo to get back on the grid

            int modx = nx % gridX;
            if (modx < 0) { modx = gridX + modx; }

            int mody = ny % gridY;
            if (mody < 0) { mody = gridY + mody; }

            Bot nb{};
            nb.startX = modx;
            nb.startY = mody;
            nb.velX = b.velX;
            nb.velY = b.velY;

            result.emplace_back(nb);
        }

        // safety score
        int tl = 0;
        int tr = 0;
        int bl = 0;
        int br = 0;
        for (auto& b : result) {
            int cx = gridX / 2;
            int cy = gridY / 2;

            if (b.startX < cx) { // left side
                if (b.startY < cy) { // top side
                    ++tl;
                } else if (b.startY > cy) { // bottom side
                    ++bl;
                }
            } else if (b.startX > cx) { // r side
                if (b.startY < cy) { // top side
                    ++tr;
                } else if (b.startY > cy) { // bottom side
                    ++br;
                }
            }
        }

        // std::cout << tl << ", " << tr << ", " << bl << ", " << br << "\n";

        reportSolution(tl * bl * tr * br);
    }

    bool tree(const std::set<std::pair<int,int>>& pos) const {

        if (pos.contains({gridX / 2, 0})) {
            // for (int y = 0; y < gridY; ++y) {
            //     for (int x = 0; x < gridX; ++x) {
            //         std::cout << (pos.contains({x,y}) ? '#' : '.');
            //     }
            //     std::cout << "\n";
            // }
            // std::cout << "\n\n";
        }

        return false;
    }

    void step (std::set<std::pair<int,int>>& out, std::vector<Bot>& b) const {
        out.clear();
        for (auto& bot : b) {
            bot.startX = bot.startX + bot.velX;
            bot.startY = bot.startY + bot.velY;

            bot.startX = bot.startX % gridX;
            if (bot.startX < 0) { bot.startX = gridX + bot.startX; }
            bot.startY = bot.startY % gridY;
            if (bot.startY < 0) { bot.startY = gridY + bot.startY; }

            out.emplace(bot.startX, bot.startY);
        }
    }

    void print_bots(const std::set<std::pair<int, int>> & set, std::ofstream & output, int sec) const {
        static auto print = [&]() {
            for (int y = 0; y < gridY; ++y) {
                for (int x = 0; x < gridX; ++x) {
                    output << (set.contains({x,y}) ? '#' : '.');
                }
                output << "\n";
            }
            output << "\n\n";
        };

        // hardcoded checks to inspect the txt file for patterns manually.
        if ((sec - 48) % 101 == 0) {
            output << sec << "\n";
            print();
        }
    };

    void v2() const override {
        std::set<std::pair<int,int>> botpos;
        std::vector<Bot> mut_bots = bots;

        for (auto& b : bots) {
            botpos.emplace(b.startX, b.startY);
        }
        // int total = 0;
        // for (int i = 1; i <= 51; i += 2) {
        //     total += i;
        // }
        // std::cout << total << "\n";

        std::ofstream output ("day14.txt");
        int sec = 0;
        while (! tree(botpos)) {

            print_bots(botpos, output, sec);
            step(botpos, mut_bots);
            ++sec;

            if (sec > 100000) break;
            // std::cout << sec << "\n";
        }


        // concluded with the E.Y.E.B.A.L.L. Algorithm.
        // Every
        // Year
        // Eyes
        // Beat
        // Algorithms,
        // Least
        // LOnce
        reportSolution(7623);
    }

    void parseBenchReset() override {
        bots.clear();
    }

    private:
    int gridX = 101;
    int gridY = 103;
    std::vector<Bot> bots;
};

} // namespace

#undef DAY