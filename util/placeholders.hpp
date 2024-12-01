#pragma once

#include "Day.hpp"
#include <stdexcept>
#include <filesystem>
#include "macros.hpp"

#define PLACEHOLD(DAY) NAMESPACE_DEF(DAY) { CLASS_DEF(DAY) { \
public: DEFAULT_CTOR_DEF(DAY)                       \
    void parse(std::ifstream&) override {           \
        throw std::runtime_error("Not Implemented");\
    }                                               \
    void parseBenchReset() override {               \
        throw std::runtime_error("Not implemented");\
    }                                               \
    void v1() const override {                      \
        throw std::runtime_error("Not Implemented");\
    }                                               \
    void v2() const override {                      \
        throw std::runtime_error("Not Implemented");\
    }                                               \
};}
