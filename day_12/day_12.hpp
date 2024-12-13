#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 12

NAMESPACE_DEF(DAY) {

struct Blob {
    int area = 0;
    int perimeter = 0;
    char type = '?';

    std::set<std::pair<int,int>> pointcloud;

    [[nodiscard]] int get_sides() const { // n-sides is the number of 90 degree angles that we can detect in the point cloud.

        int sides = 0;

        // std::cout << " blob " << type << ":\n";


        for (auto [y,x] : pointcloud) {
            // std::cout << "\tconsider " << y << ", " << x << "\n";
            // 90 degree outer angles
            if ((! pointcloud.contains({y, x-1})) && (! pointcloud.contains({ y-1, x }))) { // 90 degree angle outer top left
                // std::cout << "\t\tOTL\n";
                sides++;
            }
            if ((! pointcloud.contains({y, x+1})) && (! pointcloud.contains({ y-1, x}))) { // 90 degree angle outer top right
                // std::cout << "\t\tOTR\n";
                sides++;
            }
            if ((! pointcloud.contains({y, x-1})) && (! pointcloud.contains({ y+1, x }))) { // 90 degree angle bot top left
                // std::cout << "\t\tOBL\n";
                sides++;
            }
            if ((! pointcloud.contains({y, x+1})) && (! pointcloud.contains({ y+1, x }))) { // 90 degree angle bot top right
                // std::cout << "\t\tOBR\n";
                sides++;
            }

            // 90 degree inner angles
            if (
                pointcloud.contains({y, x-1}) &&
                pointcloud.contains({ y-1, x }) &&
                    (! pointcloud.contains({y-1, x-1}))
                ) { // inner corner top left
                // std::cout << "\t\tITL\n";
                ++sides;
            }
            if (
                pointcloud.contains({y, x+1}) &&
                pointcloud.contains({ y-1, x }) &&
                    (! pointcloud.contains({y-1, x+1}))
                ) { // inner corner top right
                // std::cout << "\t\tITR\n";
                ++sides;
            }
            if (
                pointcloud.contains({y, x-1}) &&
                pointcloud.contains({ y+1, x }) &&
                    (! pointcloud.contains({y+1, x-1}))
                ) { // inner corner bot left
                // std::cout << "\t\tIBL\n";
                ++sides;
            }
            if (
                pointcloud.contains({y, x+1}) &&
                pointcloud.contains({ y+1, x }) &&
                    (! pointcloud.contains({y+1, x+1}))
                ) { // inner corner bot right
                // std::cout << "\t\tIBR\n";
                ++sides;
            }

            // std::cout << "\tnow at " << sides << " sides\n";
        }

        // std::cout << "blob " << type << " has " << sides << " sides\n";

        return sides;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Blob& b) {
    os << "Blob {\n\tArea: " << b.area << "\n\tPerimeter: " << b.perimeter << "\n\tType: " << b.type << "\n}";
    return os;
}

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            grid.emplace_back(line.begin(), line.end());
        }
    }

    void make_blob(int i, int j, std::set<std::pair<int,int>>& processed, Blob& out) const {
        // try all the neighbours of these lads, any non-matching char is added to perimeter, and we count chars.

        if ((i < 0 || i >= grid.size()) || (j < 0 || j >= grid[i].size())) { // out of bounds, so not a matching char :)
            out.perimeter += 1;
            return;
        }

        if (grid[i][j] != out.type) { // non-matching neighbour!
            out.perimeter += 1;
            // std::cout << "\tAdds to perimeter: " << i << ", " << j << "\n";
        } else { // matching neighbour!
            if (processed.contains({i, j})) { // we have already considered this tile.
                return;
            }
            // std::cout << "\tAdds to area: " << i << ", " << j << "\n";
            out.area += 1;
            processed.emplace(i, j);
            out.pointcloud.emplace(i, j); // for problem 2!
            // recursion: try all the neighbours.
            make_blob(i + 1, j, processed, out);
            make_blob(i - 1, j, processed, out);
            make_blob(i, j + 1, processed, out);
            make_blob(i, j - 1, processed, out);
        }
    }

    void v1() const override {
        // area: just count the chars
        // perimeter: Every piece every non-matching neighbour adds 1 perimeter.

        std::set<std::pair<int,int>> processed;
        std::vector<Blob> blobs;
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (! processed.contains({i, j})) { // new thing, let's work out this one.
                    // std::cout << " do blob " << grid[i][j] << "\n";
                    blobs.emplace_back();
                    blobs.back().type = grid[i][j];
                    make_blob(i, j, processed, blobs.back());
                }
            }
        }

        size_t price = 0;
        for (auto& b : blobs) {
            price += b.area * b.perimeter;
            // std::cout << b << "\n";
        }

        reportSolution(price);
    }

    void v2() const override { // counting sides instead. Count the right-angle corners on the blob.
        std::set<std::pair<int,int>> processed;
        std::vector<Blob> blobs;
        for (int i = 0; i < grid.size(); ++i) {
            for (int j = 0; j < grid[i].size(); ++j) {
                if (! processed.contains({i, j})) { // new thing, let's work out this one.
                    // std::cout << " do blob " << grid[i][j] << "\n";
                    blobs.emplace_back();
                    blobs.back().type = grid[i][j];
                    make_blob(i, j, processed, blobs.back());
                }
            }
        }

        size_t price = 0;
        for (auto& b : blobs) {
            price += b.area * b.get_sides();
            // std::cout << b << "\n";
        }

        reportSolution(price);
    }

    void parseBenchReset() override {
        grid.clear();
    }

    private:
    std::vector<std::vector<char>> grid;
};

} // namespace

#undef DAY