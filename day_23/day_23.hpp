#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 23

NAMESPACE_DEF(DAY) {

struct Computer {
    std::string id;
    std::set<std::string> connections;

    [[nodiscard]] int degree() const { return static_cast<int>(connections.size()); }
};

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::string line;
        while (std::getline(input, line)) {
            std::string self = line.substr(0, 2);
            std::string other = line.substr(3, 2);

            computers[self].id = self;
            computers[other].id = other;

            computers[self].connections.emplace(other);
            computers[other].connections.emplace(self);
        }
    }

    void get_triplets(const Computer& src, std::set<std::set<std::string>>& known) const {
        // for each connection of src
        for (auto& con : src.connections) {
            // find another one we have in common. That is a triplet.
            auto& other = computers.find(con)->second;

            for (auto& c2 : other.connections) {
                std::set sorted { src.id, con, c2 };
                if (src.connections.contains(c2) && ! known.contains(sorted)) {
                    known.emplace(std::move(sorted));
                }
            }
        }
    }

    void increase_k_group(const std::set<std::set<std::string>>& k_minus_one_group, std::set<std::set<std::string>>& k_group) const {
        for (auto& group : k_minus_one_group) {
            auto& someone = *group.begin();
            auto& computer_in_group = computers.find(someone)->second; // arbitrarily chosen to be the first.
            // If there exists a member in connections not in the group but contained by everyone, the k-1 group can be expanded to a k-group.
            for (auto& con : computer_in_group.connections) {
                if (group.contains(con)) continue; // it's already in the group.

                // it is not in the group, does everyone also contain it?
                bool everyone_has_it = true;
                for (auto& member : group) {
                    auto& other_pc = computers.find(member)->second;

                    if (! other_pc.connections.contains(con)) {
                        everyone_has_it = false;
                        break;
                    }
                }

                if (everyone_has_it) {
                    auto copy = group; // very efficient hmm yes.
                    copy.emplace(con);

                    // emplacing it is fine even if duplicate, the chad std::set uses operator== to not double up. The contains check also does, so why bother?
                    k_group.emplace(std::move(copy));
                }
            }
        }
    }

    void v1() const override {
        std::set<std::set<std::string>> lex_sorted_triplets;
        for (auto& [key, value] : computers) {
            if (key[0] != 't') continue;

            get_triplets(value, lex_sorted_triplets);
        }
        reportSolution(lex_sorted_triplets.size());
    }

    void v2() const override {
        // get the 't' - triplets
        std::set<std::set<std::string>> lex_sorted_triplets;
        for (auto& [key, value] : computers) {
            get_triplets(value, lex_sorted_triplets);
        }

        auto k_minus_one_group = std::move(lex_sorted_triplets);
        while (true ) {
            std::set<std::set<std::string>> next;
            increase_k_group(k_minus_one_group, next);

            if (! next.empty()) {
                k_minus_one_group = std::move(next);
            } else {
                break;
            }
        }

        assert(k_minus_one_group.size() == 1);

        std::ostringstream pwd;
        std::string sep;
        for (auto& pc : *k_minus_one_group.begin()) {
            pwd << sep << pc;
            sep = ",";
        }

        reportSolution(pwd.str()); // manually remove the last comma.
    }

    void parseBenchReset() override {
        computers.clear();
    }

    private:
    std::map<std::string, Computer> computers;
};

} // namespace

#undef DAY