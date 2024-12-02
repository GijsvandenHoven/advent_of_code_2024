/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>

enum class DeltaDir {
    DOWN,
    UP,
    ZERO,
};

struct Report {
    explicit Report(const std::string& input) {
        std::stringstream ss(input);
        while (! ss.eof()) {
            int i;
            ss >> i;
            data.emplace_back(i);
        }

        int ds = data[0] - data[1];
        if (ds > 0) { dir = DeltaDir::UP; }
        else if (ds < 0) { dir = DeltaDir::DOWN; }
        else {
            ds = data[1] - data[2];
            if (ds == 0) {
                dir = DeltaDir::ZERO;
            } else {
                dir = ds > 0 ? DeltaDir::UP : DeltaDir::DOWN;
            }
        }
    }

    int deltaLower = 1;
    int deltaHigher = 3;
    DeltaDir dir;

    bool deltaInLimit(int first, int second) {
        int delta = std::abs(data[first] - data[second]);

        return delta >= deltaLower && delta <= deltaHigher;
    }

    bool deltaMonotonic(int first, int second) {
        if (dir == DeltaDir::ZERO) return false;

        int delta = data[first] - data[second];

        if (delta == 0) return false;

        if (delta > 0 && dir == DeltaDir::DOWN) return false;

        if (delta < 0 && dir == DeltaDir::UP) return false;

        return true;
    }

    // todo: actual skip mechanic
    bool reportOK(int tolerance) {
        for (size_t i = 0; i < data.size()-1; ++i) {
            bool ok = deltaInLimit(i,i+1) && deltaMonotonic(i,i+1);

            if (!ok) {
                tolerance--;
            }

            if (tolerance < 0) {
                return false;
            }
        }

        return true;
    }

    std::vector<int> data;
};

inline std::ostream& operator<< (std::ostream& os, const Report& r) {
    os << "Report  {\n\t";
    for (auto& rr : r.data) {
        os << rr << ", ";
    }
    os << "\n}";
    return os;
}

std::vector<Report> reports;

    void parse(std::ifstream &input) {
        std::string line;
        while (std::getline(input, line)) {
            reports.emplace_back(line);
        }

        std::cout << reports.back() << "\n";
    }

void v1() {
    int safe = 0;
    for (auto& report : reports) {
        safe += report.reportOK(0);
    }

    std::cout << safe << "\n";
}

void v2() {
    int safe = 0;
    for (auto& report : reports) {
        safe += report.reportOK(1);
    }
    std::cout << safe << "\n";
}

int main()
{
    std::cout<<"Hello World\n";
    std::ifstream file("d2.txt");
    parse(file);

    v1();
    v2();

    return 0;
}