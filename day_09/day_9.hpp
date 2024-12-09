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

        void v1() const override {
            // just use a vec i think the memory footprint is ok
            std::vector<int> result;
            result.reserve(data.size() * 10); // upper bound

            bool file = true;
            int file_id = 0;
            for (auto v : data) {
                int enter = file ? file_id : -1;

                for (int i = 0; i < v; ++i) {
                    result.emplace_back(enter);
                }

                if (file) {
                    ++ file_id;
                }
                file = ! file;
            }

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

            for (auto v : result) {
                std::cout << v << ";";
            }
            std::cout << "\n";

            size_t checksum = 0;
            for (int i = 0; i < result.size(); ++i) {
                if (result[i] == -1) break;

                checksum += i * result[i];
            }

            // int left = 0;
            // int right = static_cast<int>(data.size() - 1);
            //
            // auto consume_left = [&](int index) {
            //     int v = data[index];
            //     for (int i = 0; i < v; ++i) {
            //         result.emplace_back(v);
            //     }
            // };
            //
            // consume_left(left);
            // ++left;
            //
            // while (left < right) {
            //     // consume a block on the rhs.
            //
            //     // 3 things possible: run out perfectly.
            //     // run out with space (left) remaining.
            //     // run out of left space with right remaining.
            // }


            // int left = 0;
            // int right = static_cast<int>(data.size() - 1);
            // int out_idx = 0;
            //
            // // int highest_file_id = (static_cast<int>(data.size()) / 2);
            // auto file_id = [](int x){ return x / 2; };
            //
            // size_t checksum = 0;
            //
            // auto consume_left_block = [&](int index) {
            //     int left_block_size = data[index];
            //     for (int i = 0; i < left_block_size; ++i) {
            //         checksum += file_id(index) * out_idx;
            //         ++out_idx;
            //     }
            // };
            //
            // // eat the first block.
            // consume_left_block(left);
            // ++left;
            //
            //
            // while (left < right) {
            //     // consume on the right
            //
            //     // if (left == right) { // todo
            //     //     break;
            //     // }
            //     //
            //     // // move file on right
            //     // int empty_space_left = data[left];
            //     // int file_on_right = data[right];
            //     // for (int i = 0; i < empty_space_left; ++i) {
            //     //     checksum += file_id(right) * out_idx;
            //     //     ++out_idx;
            //     //     --file_on_right;
            //     //
            //     //     if (file_on_right == 0) {
            //     //         right -= 2;
            //     //         break;
            //     //     }
            //     // }
            //     //
            //     // if (empty_space_left > 0) {
            //     //
            //     // }
            // }

            reportSolution(checksum);
        }

        void v2() const override {
            reportSolution(0);
        }

        void parseBenchReset() override {

        }

        private:
        std::vector<int> data;
    };

} // namespace

#undef DAY