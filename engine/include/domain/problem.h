#pragma once
#include <vector>
#include "zone.h"
#include "candidate.h"

class Problem{
private:
    std::vector<Zone>zones;
    std::vector<Candidate>candidates;
    std::vector<std::vector<double>>distances;
    int Budget;
public:
    Problem(const std::vector<Zone>&Zones,const std::vector<Candidate>&Candidates,
            const std::vector<std::vector<double>>&Distances, int budget)
    :zones(Zones),
    candidates(Candidates),
    distances(Distances),
    Budget(budget)
    {}

    ~Problem() = default;

    const std::vector<Zone>& getZones() const {return zones;}
    const std::vector<Candidate>& getCandidates() const {return candidates;}
    const std::vector<std::vector<double>>& getDistances() const {return distances;}
    int getBudget() const {return Budget;}

};