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

    D,
    DO,
    DO_PAREN,
    DO_FUNC,
    DON,
    DON_APO,
    DON_APO_T,
    DON_APO_T_PAREN,
    DON_APO_T_FUNC
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
        state_machine_p1[State::M] = [](char c, MachineState& m) -> State { switch (c) { case 'u': return State::MU; case 'm': return State::M; default: return State::NONE; } };
        state_machine_p1[State::MU] = [](char c, MachineState& m) -> State { switch (c) { case 'l': return State::MUL; case 'm': return State::M; default: return State::NONE; } };
        state_machine_p1[State::MUL] = [](char c, MachineState& m) -> State { switch (c) { case '(': return State::MUL_PAREN; case 'm': return State::M; default: return State::NONE; } };
        state_machine_p1[State::MUL_PAREN] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 = 0;
                m.v1 += c - '0';
                return State::DIGIT_1_1;
            case 'm':
                return State::M;
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
            case 'm':
                return State::M;
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
            case 'm':
                return State::M;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_1_3] = [](char c, MachineState& m) -> State { switch (c) { case ',': return State::COMMA; default: return State::NONE; } };
        state_machine_p1[State::COMMA] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 = 0;
                m.v2 += c - '0';
                return State::DIGIT_2_1;
            case 'm':
                return State::M;
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
            case 'm':
                return State::M;
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
            case 'm':
                return State::M;
            default:
                return State::NONE;
        } };
        state_machine_p1[State::DIGIT_2_3] = [](char c, MachineState& m) -> State { switch (c) { case ')': return State::PAREN_CLOSE; case 'm': return State::M; default: return State::NONE; } };
        state_machine_p1[State::PAREN_CLOSE] = [](char c, MachineState& m) -> State { throw std::logic_error("Expected reset"); };

        // ============

        state_machine_p2[State::NONE] = [](char c, MachineState& m) -> State { switch (c) { case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::M] = [](char c, MachineState& m) -> State { switch (c) { case 'u': return State::MU; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::MU] = [](char c, MachineState& m) -> State { switch (c) { case 'l': return State::MUL; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::MUL] = [](char c, MachineState& m) -> State { switch (c) { case '(': return State::MUL_PAREN; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::MUL_PAREN] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 = 0;
                m.v1 += c - '0';
                return State::DIGIT_1_1;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_1_1] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 *= 10;
                m.v1 += c - '0';
                return State::DIGIT_1_2;
            case ',':
                return State::COMMA;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_1_2] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v1 *= 10;
            m.v1 += c - '0';
            return State::DIGIT_1_3;
            case ',':
                return State::COMMA;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_1_3] = [](char c, MachineState& m) -> State { switch (c) { case ',': return State::COMMA; default: return State::NONE; } };
        state_machine_p2[State::COMMA] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 = 0;
                m.v2 += c - '0';
                return State::DIGIT_2_1;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_2_1] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 *= 10;
                m.v2 += c - '0';
                return State::DIGIT_2_2;
            case ')':
                return State::PAREN_CLOSE;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_2_2] = [](char c, MachineState& m) -> State { switch (c) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                m.v2 *= 10;
                m.v2 += c - '0';
            return State::DIGIT_2_3;
            case ')':
                return State::PAREN_CLOSE;
            case 'm':
                return State::M;
                case 'd': return State::D;
            default:
                return State::NONE;
        } };
        state_machine_p2[State::DIGIT_2_3] = [](char c, MachineState& m) -> State { switch (c) { case ')': return State::PAREN_CLOSE; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::PAREN_CLOSE] = [](char c, MachineState& m) -> State { throw std::logic_error("Expected reset"); };

        state_machine_p2[State::D] = [](char c, MachineState& m) -> State { switch (c) { case 'o': return State::DO; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; }  };
        state_machine_p2[State::DO] = [](char c, MachineState& m) -> State { switch (c) { case 'n': return State::DON; case '(': return State::DO_PAREN; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DO_PAREN] = [](char c, MachineState& m) -> State { switch(c) { case ')': return State::DO_FUNC; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DO_FUNC] = [](char c, MachineState& m) -> State { throw std::logic_error("Expected reset"); };
        state_machine_p2[State::DON] = [](char c, MachineState& m) -> State { switch (c) { case '\'': return State::DON_APO; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DON_APO] = [](char c, MachineState& m) -> State { switch (c) { case 't': return State::DON_APO_T; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DON_APO_T] = [](char c, MachineState& m) -> State { switch (c) { case '(': return State::DON_APO_T_PAREN; case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DON_APO_T_PAREN] = [](char c, MachineState& m) -> State { switch (c) { case ')': return State::DON_APO_T_FUNC;  case 'm': return State::M; case 'd': return State::D; default: return State::NONE; } };
        state_machine_p2[State::DON_APO_T_FUNC] = [](char c, MachineState& m) -> State { throw std::logic_error("Expected reset"); };
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

            switch (s) {
                case State::PAREN_CLOSE:
                    s = State::NONE;
                    if (m.enabled) { // relevant for p2
                        sum += m.v1 * m.v2;
                    }
                    break;

                case State::DO_FUNC:
                    s = State::NONE;
                    m.enabled = true;
                    break;
                case State::DON_APO_T_FUNC:
                    s = State::NONE;
                    m.enabled = false;
                    break;
                default: break;
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