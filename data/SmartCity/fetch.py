import requests
import time
import json
# query = """
# [out:json][timeout:25];
# way["highway"="pedestrian"](46.7685,23.5875,46.7720,23.5925);
# out center;
# """

def get_query_bbox(city="cluj-napoca",type="bbox_full"):
    bbox=[]
    cities = None
    with open("data/SmartCity/cities.json","r") as f:
        cities = json.load(f)
    
    bbox = cities[city][type]
    
    return bbox

def construct_query(bbox):
    query = f"""
    [out:json][timeout:60];
    (
    way["highway"="pedestrian"]({bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]});
    way["highway"="footway"]({bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]});
    way["leisure"="park"]({bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]});
    way["place"="square"]({bbox[0]},{bbox[1]},{bbox[2]},{bbox[3]});
    );
    out geom;
    """
    
    return query

def get_data(query):
    for attempt in range(3):
        response = requests.post(
            "https://overpass.kumi.systems/api/interpreter",
            data={"data": query}
        )
        if response.status_code == 200:
            break
        print(f"Attempt {attempt+1} failed: {response.status_code}")
        time.sleep(30)

    if response.status_code == 200:
        result=response.json()
        with open("data/SmartCity/raw/streets.json","w") as f:
            json.dump(result,f,indent=2)
    else:
        print("Failed after retries")


if __name__=="__main__":
    bbox = get_query_bbox()
    query = construct_query(bbox)
    get_data(query)