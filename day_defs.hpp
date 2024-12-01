//
// Created by gijs on 11/26/24.
//

#ifndef DAY_DEFS_H
#define DAY_DEFS_H

#include "util/placeholders.hpp"

// to get main to compile. As classes for days get created, placeholders get swapped for includes.

//PLACEHOLD(1)
#include "day_01/day_1.hpp"
PLACEHOLD(2)
// #include "day_02/day_2.hpp"
PLACEHOLD(3)
// #include "day_03/day_3.hpp"
PLACEHOLD(4)
// #include "day_04/day_4.hpp"
PLACEHOLD(5)
// #include "day_05/day_5.hpp"
PLACEHOLD(6)
// #include "day_06/day_6.hpp"
PLACEHOLD(7)
// #include "day_07/day_7.hpp"
PLACEHOLD(8)
// #include "day_08/day_8.hpp"
PLACEHOLD(9)
// #include "day_09/day_9.hpp"
PLACEHOLD(10)
// #include "day_10/day_10.hpp"
PLACEHOLD(11)
// #include "day_11/day_11.hpp"
PLACEHOLD(12)
// #include "day_12/day_12.hpp"
PLACEHOLD(13)
// #include "day_13/day_13.hpp"
PLACEHOLD(14)
// #include "day_14/day_14.hpp"
PLACEHOLD(15)
// #include "day_15/day_15.hpp"
PLACEHOLD(16)
// #include "day_16/day_16.hpp"
PLACEHOLD(17)
// #include "day_17/day_17.hpp"
PLACEHOLD(18)
// #include "day_18/day_18.hpp"
PLACEHOLD(19)
// #include "day_19/day_19.hpp"
PLACEHOLD(20)
// #include "day_20/day_20.hpp"
PLACEHOLD(21)
// #include "day_21/day_21.hpp"
PLACEHOLD(22)
// #include "day_22/day_22.hpp"
PLACEHOLD(23)
// #include "day_23/day_23.hpp"
PLACEHOLD(24)
// #include "day_24/day_24.hpp"
PLACEHOLD(25)
// #include "day_25/day_25.hpp"

namespace DayMap {
    static const std::map<int, std::function<std::unique_ptr<Day>()>> NtoDay = {
        { 1, [](){ return std::make_unique<Day1::Day1>(); } },
        { 2, [](){ return std::make_unique<Day2::Day2>(); } },
        { 3, [](){ return std::make_unique<Day3::Day3>(); } },
        { 4, [](){ return std::make_unique<Day4::Day4>(); } },
        { 5, [](){ return std::make_unique<Day5::Day5>(); } },
        { 6, [](){ return std::make_unique<Day6::Day6>(); } },
        { 7, [](){ return std::make_unique<Day7::Day7>(); } },
        { 8, [](){ return std::make_unique<Day8::Day8>(); } },
        { 9, [](){ return std::make_unique<Day9::Day9>(); } },
        { 10,[](){ return std::make_unique<Day10::Day10>(); } },
        { 11,[](){ return std::make_unique<Day11::Day11>(); } },
        { 12,[](){ return std::make_unique<Day12::Day12>(); } },
        { 13,[](){ return std::make_unique<Day13::Day13>(); } },
        { 14,[](){ return std::make_unique<Day14::Day14>(); } },
        { 15,[](){ return std::make_unique<Day15::Day15>(); } },
        { 16,[](){ return std::make_unique<Day16::Day16>(); } },
        { 17,[](){ return std::make_unique<Day17::Day17>(); } },
        { 18,[](){ return std::make_unique<Day18::Day18>(); } },
        { 19,[](){ return std::make_unique<Day19::Day19>(); } },
        { 20,[](){ return std::make_unique<Day20::Day20>(); } },
        { 21,[](){ return std::make_unique<Day21::Day21>(); } },
        { 22,[](){ return std::make_unique<Day22::Day22>(); } },
        { 23,[](){ return std::make_unique<Day23::Day23>(); } },
        { 24,[](){ return std::make_unique<Day24::Day24>(); } },
        { 25,[](){ return std::make_unique<Day25::Day25>(); } },
    };

    inline auto get(int n) {
        auto iter = NtoDay.find(n);

        if (iter == NtoDay.end()) {
            throw std::logic_error(std::to_string(n) + ": This day is not valid.");
        }

        return iter->second();
    }
}

#endif //DAY_DEFS_H
