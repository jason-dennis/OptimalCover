#pragma once
#include "domain/problem.h"
#include "domain/solution.h"
#include "solvers/solver.h"

class GreedySolver: public Solver{
private:
    const Problem& problemData;

public:

    GreedySolver(const Problem& problem): problemData(problem){}
    ~GreedySolver() = default;

    Solution solve() override;

};