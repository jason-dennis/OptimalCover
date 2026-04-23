#pragma once
#include <vector>
class Solution{
private:
    const std::vector<bool>y;
    const std::vector<std::vector<double>>x;
    double objective_value;

public:

    Solution(const std::vector<bool>&Y, const std::vector<std::vector<double>>&X)
    : x(X),
    y(Y)
    {}
    ~Solution() = default;

    const std::vector<bool>& getLocations() const {return y;}
    const std::vector<std::vector<double>>& getLocationsFractions() const {return x;}
    const double getObjectiveValue() const {return objective_value;}

    void setObjectiveValue(double value) {objective_value = value;}
    

};