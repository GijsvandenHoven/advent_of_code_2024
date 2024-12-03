#pragma once

#include <iostream>

#include "../util/Day.hpp"
#include "../util/macros.hpp"

#define DAY 3

NAMESPACE_DEF(DAY) {

enum class State : uint8_t {
    NONE = 0,
    M,
    MU,
    MUL,
    MUL_PAREN,
    DIGIT_1_1, // any digit can transition to COMMA or the next digit
    DIGIT_1_2,
    DIGIT_1_3, // transition only to comma or to none
    COMMA,
    DIGIT_2_1,
    DIGIT_2_2,
    DIGIT_2_3,
    PAREN_CLOSE,
};

struct MachineState {
    int v1 = -1;
    int v2 = -1;
    bool enabled = true;
};

typedef std::map<State, std::function<State(char, MachineState&)>> FSM;

CLASS_DEF(DAY) {
    public:
    DEFAULT_CTOR_DEF(DAY)

    void parse(std::ifstream &input) override {
        std::stringstream ss;

        while (true) {
            const int x = input.get();
            if (x == EOF) break;

            ss << static_cast<char>(x);
        }

        memory = ss.str();

        state_machine_p1[State::NONE] = [](char c, MachineState& m) -> State { switch (c) { case 'm': return State::M; default: return State::NONE; } };
        state_machine_p1[State::M] = [](char c, MachineState& m) -> State { switch (c) { case 'u': return State::MU; default: return State::NONE; } };
        state_machine_p1[State::MU] = [](char c, MachineState& m) -> State { switch (c) { case 'l': return State::MUL; default: return State::NONE; } };
        state_machine_p1[State::MUL] = [](char c, MachineState& m) -> State { switch (c) { case '(': return State::MUL_PAREN; default: return State::NONE; } };
        state_machine_p1[State::MUL_PAREN] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 = 0;
                m.v1 += c - '0';
                return State::DIGIT_1_1;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_1_1] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 *= 10;
                m.v1 += c - '0';
                return State::DIGIT_1_2;
            case ',':
                return State::COMMA;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_1_2] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 *= 10;
            m.v1 += c - '0';
            return State::DIGIT_1_3;
            case ',':
                return State::COMMA;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_1_3] = [](char c, MachineState& m) -> State { switch (c) { case ',': return State::COMMA; default: return State::NONE; } };
        state_machine_p1[State::COMMA] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 = 0;
                m.v2 += c - '0';
                return State::DIGIT_2_1;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_2_1] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 *= 10;
                m.v2 += c - '0';
                return State::DIGIT_2_2;
            case ')':
                return State::PAREN_CLOSE;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_2_2] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 *= 10;
                m.v2 += c - '0';
            return State::DIGIT_2_3;
            case ')':
                return State::PAREN_CLOSE;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_2_3] = [](char c, MachineState& m) -> State { switch (c) { case ')': return State::PAREN_CLOSE; default: return State::NONE; } };
        state_machine_p1[State::PAREN_CLOSE] = [](char c, MachineState& m) -> State { throw std::logic_error("Expected reset"); };

        state_machine_p2 = state_machine_p1;
    }

    int solve(const FSM& fsm) const {
        std::stringstream ss (memory);
        MachineState m;
        State s = State::NONE;

        int sum = 0;
        do {
            int c = ss.get();
            if (c == EOF) break;

            auto iter = fsm.find(s);
            if (iter == fsm.end()) {
                throw std::logic_error("Unknown enum value");
            }

            s = iter->second(static_cast<char>(c), m);

            if (s == State::PAREN_CLOSE) {
                s = State::NONE;
                if (m.enabled) { // relevant for p2
                    sum += m.v1 * m.v2;
                }
            }
        } while (true);

        return sum;
    }

    void v1() const override {
        reportSolution(solve(state_machine_p1));
    }

    void v2() const override {
        reportSolution(solve(state_machine_p2));
    }

    void parseBenchReset() override {

    }

    private:
    std::string memory;
    FSM state_machine_p1;
    FSM state_machine_p2;
};

} // namespace

#undef DAY