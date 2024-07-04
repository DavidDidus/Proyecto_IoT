import time
import random
import requests

url = "http://127.0.0.1:8050/update_data"

while True:
    data = {
        "tomato": random.uniform(20, 30),
        "lettuce": random.uniform(40, 60),
        "spinach": random.uniform(50, 70),
        "temperature": random.uniform(15, 35),
        "humidity_air": random.uniform(30, 90)
    }
    requests.post(url, json=data)
    time.sleep(5)
