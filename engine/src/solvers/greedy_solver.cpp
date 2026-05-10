#include "solvers/greedy_solver.h"
#include "dataStructure/CandidateStructure.h"
/*
Greedy final form

Order demands — by size descending, large demands first to avoid packing pathology
For each demand:

Try candidate routers in distance order
First one that's open with enough remaining capacity — assign it
If none open fit — open the closest one that fits, deduct 2000 from budget
If can't open within budget — infeasible

Return solution
        
*/
Solution GreedySolver::solve(){

    std::vector<Zone> zones = problemData.getZones();
    std::vector<Candidate> candidates = problemData.getCandidates();
    std::vector<std::vector<double>> distances = problemData.getDistances();
    int Budget = problemData.getBudget();

    std::vector<bool> y(candidates.size());
    std::vector<std::vector<double>> x(zones.size(),std::vector<double>(candidates.size()));


    sort(zones.begin(),zones.end(),[&](const Zone& a, const Zone& b){
        return a.connections > b.connections;
    });

    CandidateStructure inactive(candidates);
    CandidateStructure active;

    double radius_m = 100.0;
    bool solution = true;

    int Cost = 0;
    
    for(const auto& zone : zones){
        if(!solution) break;
        double dlat = radius_m / 111000.0;
        double dlon = radius_m / (111000.0 * std::cos(zone.lat * M_PI / 180.0));
        int need = zone.connections;
        double clat = zone.lat;
        double clon = zone.lon;

        //check for active candidates
        while(need){
            auto closest = active.query(clat-dlat, clat+dlat,
                   clon-dlon, clon+dlon,
                   clat, clon, need);
            if(!closest.p) break;
            int remaining = closest.p->capacity - closest.used;
            double fraction = (double)closest.used / (double)zone.connections;

            need -= closest.used;

            active.update(*closest.p,remaining);

            x[zone.id][closest.p->id] += fraction;
        }

        //check for inactive candidates
        while(need){
            auto closest = inactive.query(clat-dlat, clat+dlat,
                   clon-dlon, clon+dlon,
                   clat, clon, need);
            if(!closest.p) {
                solution = false;
                break;
            }
            if(Cost + closest.p->fixed_cost > Budget){
                solution = false;
                break;
            }

            int remaining = closest.p->capacity - closest.used;
            double fraction = (double)closest.used / (double)zone.connections;
            x[zone.id][closest.p->id] += fraction;
            y[closest.p->id] = true;

            need -= closest.used;
            Cost += closest.p->fixed_cost;

            Candidate copy = *closest.p;
            inactive.remove(copy);
            if(remaining > 0)
            {
                copy.capacity = remaining;
                active.insert(copy);
            }
        }
    }

    if(!solution){
        throw NoFeasibleSolutionException(
        "Greedy solver: no feasible solution found. "
        "Budget too low or coverage radius too small."
        ); 
    }


    Solution sol(y,x);
    return sol;
}