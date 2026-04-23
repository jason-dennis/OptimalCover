#include "domain/WIFIobjective.h"

double WifiObjective::evaluate(const Problem & P, const Solution & S)
{

    const std::vector<Zone>& zones = P.getZones();
    const std::vector<Candidate>& candidates = P.getCandidates();
    const std::vector<std::vector<double>>& distances = P.getDistances();

    const std::vector<bool>& y = S.getLocations();
    const std::vector<std::vector<double>>& x =S.getLocationsFractions();

    double Z = 0.0;

    for(int i = 0; i<zones.size(); ++ i){
        for(int j = 0; j<candidates.size(); ++j){
            Z += (zones[i].connections * distances[i][j] * x[i][j]);
        }
    }

    for(int j = 0; j<candidates.size(); ++j){
        Z += (candidates[j].fixed_cost * y[j]);
    }

    return Z;
}