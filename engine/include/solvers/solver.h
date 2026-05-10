#pragma once
#include "domain/problem.h"
#include "domain/solution.h"
#include <stdexcept>

class NoFeasibleSolutionException : public std::runtime_error {
public:
    explicit NoFeasibleSolutionException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class Solver{
public:

    virtual ~Solver() = default;

    virtual  Solution solve() = 0;

};