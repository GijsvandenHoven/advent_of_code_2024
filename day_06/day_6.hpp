#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 6

NAMESPACE_DEF(DAY) {

enum class Facing {
    N = 0,
    S,
    E,
    W
};

struct GridNavigator {
    virtual ~GridNavigator() = default;

    std::pair<int, int> pos = {-1,-1};
    Facing f = Facing::N;

    [[nodiscard]] std::pair<int,int> step() const {
        auto [x,y] = pos;
        switch (f) {
            case Facing::N:
                --y;
                break;
            case Facing::S:
                ++y;
                break;
            case Facing::W:
                --x;
                break;
            case Facing::E:
                ++x;
                break;
            default: throw std::logic_error("Non-Exhaustive enum switch");
        }
        return { x, y };
    }

    bool rotateRight() {
        // add this rotation to memory.
        auto [_, insertion] = memory.emplace(f, pos);
        if (! insertion) { // we already did this rotation in this spot, this is a loop.
            looped = true;
            return false;
        }

        switch (f) {
            case Facing::N:
                f = Facing::E;
                break;
            case Facing::S:
                f = Facing::W;
                break;
            case Facing::W:
                f = Facing::N;
                break;
            case Facing::E:
                f = Facing::S;
                break;
            default:
                throw std::logic_error("Non-Exhaustive enum switch");
        }

        return true; // overrides might return otherwise.
    }

    [[nodiscard]] bool on_loop() const { return looped; }
private:
    bool looped = false;
    std::set<std::pair<Facing, std::pair<int, int>>> memory;
};

class Grid : public std::vector<std::vector<char>> {
public:
    Grid() = default;

    void addLine(const std::string& l) {
        this->emplace_back(l.begin(), l.end());
        size_t x;
        if ((x = l.find_first_of('^')) != std::string::npos) {
            guard.pos = { x, this->size() - 1 };
            this->back()[x] = '.';
            guard_start_pos = guard.pos; // needed for problem 2.
        }
    }

    bool guardStep() {
        const auto suggested = guard.step();
        auto [x,y] = suggested;

        if (! inBounds(suggested)) {
            return false;
        }

        bool ok = true;
        if (this->operator[](y)[x] == '#') { // collision.
            ok = guard.rotateRight();
        } else { // collision
            guard.pos = suggested;
        }

        return ok;
    }

    void markGuard() {
        this->operator[](guard.pos.second)[guard.pos.first] = 'X';
    }

    auto guardStartPos() { return this->guard_start_pos; }

    void assignGuard(GridNavigator&& g) {
        guard = g;
    }

    [[nodiscard]] bool on_loop() const {
        return guard.on_loop();
    }

private:
    [[nodiscard]] bool inBounds (const std::pair<int,int>& p) const {
        const auto [x,y] = p;
        if (y < 0 || y > this->size() -1) {
            return false;
        }

        if (x < 0 || x > this->operator[](y).size() -1) {
            return false;
        }

        return true;
    }

    GridNavigator guard;

    std::pair<int,int> guard_start_pos;

    friend std::ostream& operator<<(std::ostream& os, const Grid& g);
};

inline std::ostream& operator<<(std::ostream& os, const Grid& g) {
    os << "Grid {\n\t";
    os << "Guard: { (" << g.guard.pos.first << ", " << g.guard.pos.second << ") F: " << (
        g.guard.f == Facing::N ? 'N' : g.guard.f == Facing::S ? 'S' : g.guard.f == Facing::E ? 'E' : g.guard.f == Facing::W ? 'W' : '?'
    ) << " }\n\t";
    for (auto& l : g) {
        for (auto c : l) {
            os << c;
        }
        os << "\n\t";
    }
    os << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;

        while (std::getline(input, line)) {
            g.addLine(line);
        }
    }

    void v1() const override {
        Grid copy = g; // immutability.

        bool stepSuccess;
        do {
            copy.markGuard();
            stepSuccess = copy.guardStep();
        } while (stepSuccess);

        int sum = 0;
        for (auto& l : copy) {
            for (auto c : l) {
                sum += c == 'X';
            }
        }

        reportSolution(sum);
    }

    void v2() const override {
        Grid copy = g; // immutability.

        bool stepSuccess;
        do {
            copy.markGuard();
            stepSuccess = copy.guardStep();
        } while (stepSuccess);

        int sum = 0;
        // use the 'X' marks to try placing a new obstacle, then step to try and find loops. Skip the guard starting position.
        for (size_t i = 0; i < copy.size(); ++i) {
            for (size_t j = 0; j < copy[i].size(); ++j) {
                auto c = copy[i][j];
                if (c != 'X') { // guard does not touch this spot, placing an obstacle here does nothing.
                    continue;
                }
                if (std::make_pair(static_cast<int>(j), static_cast<int>(i)) == copy.guardStartPos()) { // can't place here.
                    continue;
                }

                // try marking this X spot and see if there is a loop.
                copy[i][j] = '#';

                // new guard in the start position to see if we loop.
                GridNavigator newGuard;
                newGuard.pos = copy.guardStartPos();
                copy.assignGuard(std::move(newGuard));

                bool newGuardStep;
                do {
                    newGuardStep = copy.guardStep();
                } while (newGuardStep);

                if (copy.on_loop()) {
                    sum += 1;
                }

                // restore state.
                copy[i][j] = 'X';
            }
        }

        reportSolution(sum);
    }

    void parseBenchReset() override {
        g.clear();
    }

    private:
    Grid g;
};

} // namespace

#undef DAY