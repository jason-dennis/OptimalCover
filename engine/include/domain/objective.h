#pragma once
#include "problem.h"
#include "solution.h"

class Objective{

public:
    Objective() = default;
    virtual ~Objective() = default; 
    
    virtual double evaluate(const Problem&, const Solution&) = 0;

};