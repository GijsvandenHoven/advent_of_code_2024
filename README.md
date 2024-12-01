This template allows the easy creation of classes implementing AoC solutions, by deriving from a shared `Day` class.
The design is such that a program argument (number) controls which day (1-25) of the puzzles should be run.

The template also contains functionality to benchmark performance of a solution.

# Workflow / Use of this template:

## When you are adding a solution for a day (`day N`)

1. Create a folder named `day_NN`  where `N` is your day number extended to two digits (ex. `day_02`, `day_11`)
2. In the folder create a file named `dayN.txt`. Here you must paste the program input provided by AoC.
3. In the folder create a blank file named `day_N.hpp`.
    1. Copy-paste into this the contents of `_template/template.cpp`
    2. Replace the `#define DAY 0` with `#define DAY N`.
4. In day_defs.hpp replace swap `PLACEHOLD(N)` with its commented-out counterpart `#include "day_NN/day_N.hpp"`.

A basic example of this is given in the template by already creating Day 1 with this flow.

## When you are writing a solution for a day

*You will usually be able to implement a solution with just the `day_N.hpp` header file, especially if you are willing to define supporting structs and operator overloads (if needed) in the same file.
If you need to use extra files, remember to update the `CMakeLists.txt`. You would need to update the `add_executable` line to reference any newly added source (`.cpp`) files.*

First implement a solution for parsing the program input. 
The superclass creates an input stream of the `dayN.txt` file of the folder it is in, and hands it to the `parse` function of your `dayN` class.
The purpose of this function is usually to convert the text file into relevant member variables you created for solving this problem. This is also why it is not marked `const`.

When this is done, you can implement a solution to the initial puzzle of the day in the `v1()` function.
To 'return' a solution, use the superclass `reportSolution` function. 
The only requirement of this template function is that the reported object has an `operator<<` for `std::cout` operations, but usually AoC only requires you to report numbers or strings. 

The reported solution will be printed to standard out, letting you copy it into the answer box of the puzzle.

If your answer is correct, AoC reveals a second part of the puzzle. This is usually a non-trivial twist of the problem. Implement it in `v2` just like you did for `v1`.

To enable benchmarking, the `v1()` and `v2()` functions must be immutable. Hence, they are marked const. If you need to mutate any information that was parsed into member variables by `parse()`, you must make a local copy of it, or consider a different approach.

## When you are running a solution for a day

Without any runtime arguments, the program will not run your solution. You can provide the following arguments:

`[rootFolder] [solve|bench|bench_all] [dayNumber] (bench_sample_size)`

rootFolder - String. Absolute path to your code. Needed to grab `/day_NN/dayN.txt` files.

solve|bench|bench_all - String. Execution mode. For just running a solution, use `solve`. For benchmarking (next section), use `bench`. `bench_all` is an advanced mode, refer to the `benchEverything` function in main.cpp.

dayNumber - Number. The day to operate on. For day `N`, use `N`.

bench_sample_size - Number, optional. Overrides the default 10,000 samples for bench mode.

## When you are benchmarking a solution for a day

Just use the `bench` argument instead of solve to get started! Adjust `bench_sample_size` if necessary. 

Text parsing is only done once before benchmarking `v1` and `v2` respectively, but benchmarking also tests parsing performance before this.
In case your parsing function modifies rather than sets any class member variables, multiple parsing runs will cause incorrect behavior.
**When this is the case, it is necessary to implement `parseBenchReset()`** to clear variables in between runs. It is called each time a parsing run is done during the benchmarking of `parse()`.

For the best performance results, remember to use release rather than debug build configurations.
