#pragma once
#include "objective.h"

class WifiObjective: public Objective{
public:
    WifiObjective() = default;
    ~WifiObjective() = default;

    double evaluate(const Problem&, const Solution&) override;
};