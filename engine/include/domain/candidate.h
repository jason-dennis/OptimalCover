#pragma once

struct Candidate{
    double lat,lon;
    int capacity;
    double fixed_cost;
    Candidate(double LAT,double LON,int CAPACITY,int COST)
    :lat(LAT),
    lon(LON),
    capacity(CAPACITY),
       fixed_cost(COST)
    {}
};