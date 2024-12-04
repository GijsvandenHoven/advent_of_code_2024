#pragma once

#include <iostream>
#include <set>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 4

struct Grid : public std::vector<std::vector<char>> {
    const std::vector<char>& add_line(const std::string& l) {
        this->emplace_back(l.begin(), l.end());
        return this->back();
    }

    [[nodiscard]] std::pair<bool, char> get(int i, int j) const {
        if (i < 0 || i >= this->size()) {
            return std::make_pair(false, 0);
        }

        const auto& v = this->operator[](i);
        if (j < 0 || j >= v.size()) {
            return std::make_pair(false, 0);
        }

        return std::make_pair(true, v[j]);
    }

    void findAllChar(char c, std::vector<std::pair<int,int>>& results) const {
        for (int i = 0; i < this->size(); ++i) {
            auto& v = this->operator[](i);
            for (int j = 0; j < v.size(); ++j) {
                if (v[j] == c) {
                    results.emplace_back(i, j);
                }
            }
        }
    }
};

inline std::ostream& operator<<(std::ostream& os, const Grid& g) {
    os << "Grid {\n";
    for (auto& l : g) {
        std::cout << "\t";
        for (auto& c : l) {
            std::cout << c;
        }
        std::cout << "\n";
    }
    std::cout << "}";

    return os;
}

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string line;
            while (std::getline(input, line)) {
                auto& result = g.add_line(line);
                assert(!result.empty());
            }
        }

        void v1() const override {
            std::vector<std::pair<int,int>> coords;
            g.findAllChar('X', coords);

            // for each coord for each direction search for the xmas string.
            static constexpr auto _ = [](int x, int y){ return std::make_pair(x, y); };
            static constexpr auto x = [](int x) { return _(x, 0); };
            static constexpr auto y = [](int y) { return _(0, y); };
            static constexpr auto d = [](int d) { return _(d,d); };
            static constexpr auto c = [](int x) { return _(x, -x); };
            static constexpr std::array<std::array<std::pair<int,int>,4>, 8> deltas = {{
                { x(0), x(1), x(2), x(3) }, // left
                { x(0), x(-1), x(-2), x(-3) }, // right
                { y(0), y(1), y(2), y(3) }, // up
                { y(0), y(-1), y(-2), y(-3) }, // down
                { d(0), d(1), d(2), d(3) }, // diag down right
                { d(0), d(-1), d(-2), d(-3) }, // diag up left
                { c(0), c(1), c(2), c(3) }, // diag down left
                { c(0), c(-1), c(-2), c(-3) } // diag up right
            }};

            int xmas_found = 0;
            for (const auto& [xbase, ybase] : coords) {
                for (const auto& offsets : deltas) {
                    std::string seek;
                    seek.reserve(4);

                    for (const auto& [x, y] : offsets) {
                        auto [success, value] = g.get(xbase + x, ybase + y);
                        if ( ! success ) { // indexing out of bounds.
                            break; // exit this delta list, the strirng will not be completed.
                        }

                        seek += value;
                    }

                    if (seek == "XMAS") {
                        ++ xmas_found;
                    }
                }
            }

            reportSolution(xmas_found);
        }

        void v2() const override {
            // it is probably possible to unify this code in a single method and generalize what is searched for
            // by limiting the iteration range of the deltas. But I don't really feel like doing that.

            std::vector<std::pair<int,int>> coords;
            g.findAllChar('M', coords);

            // for each coord for each direction search for the xmas string.
            static constexpr auto _ = [](int x, int y){ return std::make_pair(x, y); };
            static constexpr auto x = [](int x) { return _(x, 0); };
            static constexpr auto y = [](int y) { return _(0, y); };
            static constexpr auto d = [](int d) { return _(d,d); };
            static constexpr auto c = [](int x) { return _(x, -x); };
            static constexpr std::array<std::array<std::pair<int,int>,3>, 8> deltas = {{
                { d(0), d(1), d(2) }, // diag down right
                { d(0), d(-1), d(-2) }, // diag up left
                { c(0), c(1), c(2) }, // diag down left
                { c(0), c(-1), c(-2) } // diag up right
            }};

            std::set<std::pair<int,int>> diag_mas_centerpoints;
            int x_mas_found = 0;
            for (const auto& [xbase, ybase] : coords) {
                for (int i = 0; i < deltas.size(); ++i) {
                    const auto& offsets = deltas[i];
                    std::string seek;
                    seek.reserve(3);

                    for (const auto& [x, y] : offsets) {
                        auto [success, value] = g.get(xbase + x, ybase + y);
                        if ( ! success ) { // indexing out of bounds.
                            break; // exit this delta list, the strirng will not be completed.
                        }

                        seek += value;
                    }

                    if (seek == "MAS") {
                        // the position of 'A' is at offsets[1].
                        // We can register this as the centerpoint of a 'MAS', regardless of its direction.
                        // If  we insert this centerpoint into a set, we know we have found X-MAS when the centerpoint already exists.
                        // Recall that we only searched diagonal, so we have no false positives here.
                        auto centerpoint = offsets[1];
                        centerpoint.first += xbase;
                        centerpoint.second += ybase;
                        auto [_, novel] = diag_mas_centerpoints.emplace(centerpoint);
                        if (! novel) {
                            ++x_mas_found;
                        }
                    }
                }
            }

            reportSolution(x_mas_found);
        }

        void parseBenchReset() override {

        }

        private:
        Grid g;
    };

} // namespace

#undef DAY