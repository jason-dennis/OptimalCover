import json
import math
def haversine(lat1, lon1, lat2, lon2):
    R = 6371000  # raza Pământului în metri
    phi1, phi2 = math.radians(lat1), math.radians(lat2)
    dphi = math.radians(lat2 - lat1)
    dlam = math.radians(lon2 - lon1)
    a = math.sin(dphi/2)**2 + math.cos(phi1)*math.cos(phi2)*math.sin(dlam/2)**2
    return 2 * R * math.asin(math.sqrt(a))

def build_zones(elements,factor):
    zones=[]
    zoneId = 0
    for elem in elements:
        central_lat=0
        central_lon=0
        total_len = 0
        prev =elem["geometry"][0]
        cnt=len(elem["geometry"])
        for node in elem["geometry"]:
            central_lat+=node["lat"]
            central_lon+=node["lon"]
            total_len+=haversine(node["lat"],node["lon"],prev["lat"],prev["lon"])
            prev = node
        central_lat=float(central_lat/cnt)
        central_lon=float(central_lon/cnt)
        zone={"id":zoneId,"lat":central_lat,"lon":central_lon,"length_m":total_len,"connections":int(factor*total_len)+1}
        zones.append(zone)
        zoneId += 1
    return zones

def get_elements():
    elements=None
    with open("data/SmartCity/raw/streets.json","r") as f:
        elements = json.load(f)
    return elements["elements"]


def get_bbox(zones):
    bbox=[float('inf'),float('-inf'),float('inf'),float('-inf')]
    for zone in zones:
        bbox[0]=min(bbox[0],zone["lat"])
        bbox[1]=max(bbox[1],zone["lat"])
        bbox[2]=min(bbox[2],zone["lon"])
        bbox[3]=max(bbox[3],zone["lon"])
    return bbox


def generate_candidates(bbox,AP_CAPACITY,EQUIPMENT_COST,GRID_SPACING_M):
    candidates = []
    dlat = GRID_SPACING_M / 111320
    lat_medie = (bbox[0] + bbox[1]) / 2
    dlon = GRID_SPACING_M / (111320 * math.cos(math.radians(lat_medie)))
    
    lat = bbox[0]
    while(lat <= bbox[1]):
        lon = bbox[2]
        while(lon <= bbox[3]):
            candidates.append({"lat":lat,"lon":lon,"capacity":AP_CAPACITY,"fixed_cost":EQUIPMENT_COST})
            lon+=dlon
        lat += dlat
    
    return candidates
    
def build_distances(zones,candidates):
    distances = []
    for zone in zones:
        distances_zone=[]
    
        for candidate in candidates:
            distance = haversine(zone["lat"],zone["lon"],candidate["lat"],candidate["lon"])
            distances_zone.append(distance)
        distances.append(distances_zone)
    
    return distances
            
if __name__=="__main__":
    AP_CAPACITY = 150
    EQUIPMENT_COST = 2000
    FACTOR = 0.267
    GRID_SPACING_M = 100
    elements = get_elements()
    zones = build_zones(elements,FACTOR)
    bbox = get_bbox(zones)
    candidates = generate_candidates(bbox,AP_CAPACITY,EQUIPMENT_COST,GRID_SPACING_M)
    # distances = build_distances(zones,candidates)
    
    problem_data={"zones":zones, "candidates":candidates}
    with open("data/SmartCity/output/problem.json","w") as f:
        json.dump(problem_data,f,indent=2)
    
