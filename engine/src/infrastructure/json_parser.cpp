#include "infrastructure/json_parser.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <cmath>

using json = nlohmann::json;

double haversine(double lat1, double lon1, double lat2, double lon2) {
    constexpr double R = 6371000.0;
    constexpr double TO_RAD = M_PI / 180.0;

    double phi1 = lat1 * TO_RAD, phi2 = lat2 * TO_RAD;
    double dphi = (lat2 - lat1) * TO_RAD;
    double dlam = (lon2 - lon1) * TO_RAD;

    double a = std::sin(dphi/2)*std::sin(dphi/2)
             + std::cos(phi1)*std::cos(phi2)
             * std::sin(dlam/2)*std::sin(dlam/2);

    return 2.0 * R * std::asin(std::sqrt(a));
}

Problem parse(const std::string& filepath){

    std::ifstream f(filepath);
    if (!f.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    json Data = json::parse(f);

    std::vector<Zone>zones;
    for(auto& zone : Data["zones"]){
        zones.push_back(Zone(zone["id"], zone["lat"], zone["lon"], zone["connections"]));
    }

    std::vector<Candidate>candidates;
    for(auto& candidate : Data["candidates"]){
        candidates.push_back(Candidate(candidate["id"],candidate["lat"], candidate["lon"],
                                       candidate["capacity"], candidate["fixed_cost"]));
    }

    std::vector<std::vector<double>>distances(zones.size(),std::vector<double>(candidates.size()));

    for(int i = 0; i < zones.size(); ++i){
        for(int j = 0; j < candidates.size(); ++j){
            distances[i][j] = (double) haversine(zones[i].lat, zones[i].lon, candidates[j].lat, candidates[j].lon);
        }
    }

    int Budget = Data["candidates"]["budget"];
    
    return Problem(zones, candidates, distances, Budget);

}