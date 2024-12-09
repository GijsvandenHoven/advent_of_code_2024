#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 9

NAMESPACE_DEF(DAY) {

    CLASS_DEF(DAY) {
        public:
        DEFAULT_CTOR_DEF(DAY)

        void parse(std::ifstream &input) override {
            std::string tmp;
            input >> tmp;
            data.reserve(tmp.size());

            for (auto c : tmp) {
                data.emplace_back(c - '0');
            }

            if (data.size() % 2 == 0) {
                throw std::logic_error("File size should be odd");
            }
        }

        void make_file(std::vector<int>& out) const {
            bool file = true;
            int file_id = 0;
            for (auto v : data) {
                int enter = file ? file_id : -1;

                for (int i = 0; i < v; ++i) {
                    out.emplace_back(enter);
                }

                if (file) {
                    ++ file_id;
                }
                file = ! file;
            }
        }

        static size_t checksum(const std::vector<int>& v) {
            size_t checksum = 0;
            for (int i = 0; i < v.size(); ++i) {
                if (v[i] == -1) continue;

                checksum += i * v[i];
            }

            return checksum;
        }

        void v1() const override {
            // just use a vec i think the memory footprint is ok
            std::vector<int> result;
            result.reserve(data.size() * 10); // upper bound

            make_file(result);

            int first_empty = 0;
            auto next_empty = [&]() {
                while (result[first_empty] != -1) { // dangerous if there is no empty :)
                    ++first_empty;
                }
            };

            next_empty(); // set up this index.

            for (int i = static_cast<int>(result.size() - 1); i >= 0; --i) {
                if (first_empty > i) break;

                if (result[i] == -1) continue;

                // swap first empty and i, then update first empty
                result[first_empty] = result[i];
                result[i] = -1;
                next_empty();
            }

            // for (auto v : result) {
            //     std::cout << v << ";";
            // }
            // std::cout << "\n";

            reportSolution(checksum(result));
        }

        void v2() const override {
            std::vector<int> result;
            result.reserve(data.size() * 10); // upper bound

            make_file(result);

            // for each file try to move it left
            int first_empty = 0;
            auto next_empty = [&]() {
                while (result[first_empty] != -1) { // dangerous if there is no empty :)
                    ++first_empty;
                }
            };

            next_empty(); // set up this index.

            for (int i = static_cast<int>(result.size() - 1); i >= 0; --i) {
                if (first_empty > i) break;

                if (result[i] == -1) continue;

                // get the size of this file
                int file_size = 0;
                int file_desc = result[i];
                int file_end_i = i;
                while (result[i] == file_desc) { // again quite unsafe with a full disk :)
                    // puzzle doesnt do this so i dont either.
                    // result[i] = -1; // just temporarily delete the file what could go wrong. Necessary to do accurate empty space checks.
                    ++ file_size;
                    -- i;
                }
                ++ i; // we want it to point to the start of the file we just ate.

                // seek left to right for the first empty block that would fit this.
                int block_size = 0;
                int move_to_i = -100;
                for (int j = first_empty; j <= file_end_i; ++j) {
                    if (result[j] == -1) {
                        ++block_size;
                        if (block_size == file_size) { // ok put the file here. Guaranteed to find something due to -1 setting earlier.
                            move_to_i = j;
                            break;
                        }
                    } else {
                        block_size = 0;
                    }
                }

                // std::cout << "found block of size >" << block_size << " at " << move_to_i << "(back)\n";

                if (move_to_i == -100) {
                    continue;
                    // throw std::logic_error("No movable block");
                }

                for (int k = 0; k < file_size; ++k) {
                    result[move_to_i - k] = file_desc;
                    result[i + k] = -1; // i points to start of file.
                }

                next_empty(); // may choose itself if no movement was done.
            }


            reportSolution(checksum(result));
        }

        void parseBenchReset() override {

        }

        private:
        std::vector<int> data;
    };

} // namespace

#undef DAY