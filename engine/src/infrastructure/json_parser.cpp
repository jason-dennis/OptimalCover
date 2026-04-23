#include "infrastructure/json_parser.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;


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
        candidates.push_back(Candidate(candidate["lat"], candidate["lon"],
                                       candidate["capacity"], candidate["fixed_cost"]));
    }

    std::vector<std::vector<double>>distances = Data["distances"];
    
    return Problem(zones, candidates, distances);

}