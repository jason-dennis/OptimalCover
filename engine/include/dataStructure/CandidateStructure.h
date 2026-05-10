#pragma once
#include "domain/candidate.h"
#include <vector>
#include <map>
#include <set>
#include <cmath>

struct Result {
        Candidate* p;
        double dist;
        int used;
};

struct CmpLon {
        bool operator()(const Candidate& a, const Candidate& b) const {
            if (a.lon != b.lon) return a.lon < b.lon;
            if (a.lat != b.lat) return a.lat < b.lat;
            return a.capacity < b.capacity;
        }
};

using Inner = std::multiset<Candidate, CmpLon>;

inline double haversine(double lat1, double lon1, double lat2, double lon2) {
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

class CandidateStructure{
private:
    std::map<double, Inner> tree;
public:

    CandidateStructure() = default;
    CandidateStructure(std::vector<Candidate>& candidates){
        for(const auto& c : candidates){
            insert(c);
        }
    }

    void insert(const Candidate& c){
        tree[c.lat].insert(c);
    }

    void remove(const Candidate& c){

        auto inner = tree.find(c.lat);
        if(inner == tree.end()) return;

        auto it = inner->second.find(c);
        if(it == inner->second.end()) return;

        inner->second.erase(it);

        if(inner->second.empty()) tree.erase(inner);
    }

    void update(const Candidate& c, int new_capacity){
        remove(c);
        Candidate new_c = c;
        new_c.capacity = new_capacity;
        insert(new_c);
    }

    Result query(double lat1, double lat2,
                 double lon1, double lon2,
                 double clat, double clon,
                 int    need) const
    {
        Candidate* best = nullptr;
        double bestD = 1e18;
        int used = 0;

        auto lo = tree.lower_bound(lat1);
        auto hi = tree.upper_bound(lat2);

        for(auto it = lo; it != hi; ++it){
            auto& inner = it->second;

            Candidate lo_key(-1,0,lon1,0,0);
            Candidate hi_key(-1,0,lon2,0,0);

            auto jlo = inner.lower_bound(lo_key);
            auto jhi = inner.upper_bound(hi_key);

            for(auto jt = jlo; jt != jhi; ++jt){
                if(jt->capacity == 0) continue;
                double d = haversine(jt->lat, jt->lon, clat, clon);
                if(d < bestD){
                    best = const_cast<Candidate*>(&*jt);
                    bestD = d;
                    used = std::min(need,jt->capacity);
                }
            }
        }

        return {best,bestD,used};
    }

    
};