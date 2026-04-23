#include "solvers/greedy_solver.h"

/*
    for every zone:
        while routers_capacity_used <= zone_connections:
            closest_router = find_closest_router  // only router with capcity > 0
            need = zone_connections - routers_capacity_used
            routers_capacity_used += min(need,closest_router.capacity);
            closest_router.capacity -= min(need,closest_router.capacity);
    
    find_closest_router:
        
*/
Solution GreedySolver::solve(){

    std::vector<Zone> zones = problemData.getZones();
    std::vector<Candidate> candidates = problemData.getCandidates();
    std::vector<std::vector<double>> distances = problemData.getDistances();

    std::vector<bool> y(candidates.size());
    std::vector<std::vector<double>> x(zones.size(),std::vector<double>(candidates.size()));




}