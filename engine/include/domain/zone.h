#pragma once

struct Zone{
    int id;
    double lat,lon;
    int connections;
    Zone(int ID,double LAT,double LON,int CONNECTIONS)
    :id(ID),
    lat(LAT),
    lon(LON),
    connections(CONNECTIONS)
    {}
    
};