#pragma once

struct Candidate{
    int id;
    double lat,lon;
    int capacity;
    double fixed_cost;
    Candidate(int ID,double LAT,double LON,int CAPACITY,int COST)
    : id(ID),
    lat(LAT),
    lon(LON),
    capacity(CAPACITY),
    fixed_cost(COST)
    {}
};