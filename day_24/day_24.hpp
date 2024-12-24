#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 24

NAMESPACE_DEF(DAY) {

inline std::map<std::string, std::function<int(int,int)>> ops = {
    {"AND", [](int a, int b){ return a == 1 && b == 1; }},
    {"OR", [](int a, int b){ return a == 1 || b == 1; }},
    {"XOR", [](int a, int b){ return a != b; }},
};

struct Gate {
    std::string lbl;
    int out = -1;

    std::string first;
    std::string second;

    std::function<int(int,int)> op = [](int,int) -> int { throw std::logic_error("Unassigned func"); };

    [[nodiscard]] bool is_source() const {
        return first.empty() && second.empty();
    }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        // std::map<std::string, int> initials;
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) break; // newline separating 1st and 2nd block

            // initials[line.substr(0, 3)] = std::stoi(line.substr(5, 1));
            auto name = line.substr(0, 3);
            gates[name].out = std::stoi(line.substr(5, 1));
            gates[name].lbl = name;
        }

        while (std::getline(input, line)) { // gate definitions.
            std::stringstream ss (line);
            std::ostringstream read;

            std::string lbl1;
            std::string op;
            std::string lbl2;
            std::string lbl3;

            while (ss.peek() != ' ') {
                read << static_cast<char>(ss.get());
            }
            ss.get();
            lbl1 = read.str();
            read.str("");

            while (ss.peek() != ' ') {
                read << static_cast<char>(ss.get());
            }
            ss.get();
            op = read.str();
            read.str("");

            while (ss.peek() != ' ') {
                read << static_cast<char>(ss.get());
            }
            ss.get();
            lbl2 = read.str();
            read.str("");

            while (ss.peek() != ' ') { ss.get(); } // arrow
            ss.get();

            while (ss.peek() != EOF) {
                read << static_cast<char>(ss.get());
            }
            lbl3 = read.str();
            read.str("");

            // std::cout << "gate: " << lbl1 << " " << op << " " << lbl2 << " -> " << lbl3 << "\n";
            gates[lbl3].op = ops.find(op)->second;
            gates[lbl3].first = lbl1;
            gates[lbl3].second = lbl2;
            gates[lbl3].lbl = lbl3;
        }

        for (auto& [k,v] : gates) {
            std::cout << k << "(" << v.lbl << "): " << v.first << ", " << v.second << " -> " << v.out << "\n";
        }
    }

    static int64_t get_value(const std::unordered_map<std::string, Gate>& values) {
        auto to_str = [](int i) {
            if (i < 10) return "z0" + std::to_string(i);
            return "z" + std::to_string(i);
        };

        int64_t i = 0;
        int64_t out = 0;
        std::unordered_map<std::string, Gate>::const_iterator x;
        while ((x = values.find(to_str(i))) != values.end()) {
            out += static_cast<int64_t>(x->second.out) << i;
            ++i;
        }

        return out;
    }

    void v1() const override {
        std::unordered_map<std::string, int> availability;
        auto copy = gates;
        for (auto& [k,v] : copy) {
            if (v.is_source()) continue;

            availability[k] = 0;

            if (copy.find(v.first)->second.out != -1) {
                availability[k]++;
            }
            if (copy.find(v.second)->second.out != -1) {
                availability[k]++;
            }
        }

        for (auto& [k, v] : availability) {
            std::cout << k << ", " << v << "\n";
        }

        while (! availability.empty()) {
            auto iter = std::find_if(availability.begin(), availability.end(), [](auto& entry){ return entry.second == 2; });
            std::cout << "consider " << iter->first << "\n";
            auto& outgate = copy.find(iter->first)->second;
            auto& ingate1 = copy.find(outgate.first)->second;
            auto& ingate2 = copy.find(outgate.second)->second;

            if (ingate1.out == -1 || ingate2.out == -1) throw std::logic_error("cannot be.");

            outgate.out = outgate.op(ingate1.out, ingate2.out);
            std::cout << outgate.lbl << " is set to " << outgate.out << "\n";
            availability.erase(iter);

            // only do this after erasing, or you invalidate the iterator.
            for (auto& [k,v] : availability) {
                auto& affected = copy.find(k)->second;
                if (affected.first == outgate.lbl || affected.second == outgate.lbl) {
                    std::cout << "This affects " << affected.lbl << " who was " << v << "\n";
                    v++;
                }
            }
        }

        // too low: 1499957451
        reportSolution(get_value(copy));
    }

    void v2() const override {
        reportSolution(0);
    }

    void parseBenchReset() override {
        gates.clear();
    }

    private:
        std::unordered_map<std::string, Gate> gates;
};

} // namespace

#undef DAY