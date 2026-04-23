#pragma once
#include "domain/problem.h"
#include "domain/solution.h"

class GreedySolver{
private:
    const Problem& problemData;

public:

    GreedySolver(const Problem& problem): problemData(problem){}
    ~GreedySolver() = default;

    Solution solve();

};