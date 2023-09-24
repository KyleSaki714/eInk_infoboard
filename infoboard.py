import time
import requests
import random as rand
from pprint import pprint
import json
import datetime

settings = {
    'api_key':'94d9fc275a89afcc4bff8278e5b04ae6',
    'zip_code':'98118',
    'country_code':'us',
    'temp_unit':'imperial'} #unit can be metric, imperial, or kelvin

pkmn_id = rand.randint(0, 1017)

datetimenow = datetime.datetime.now()
date = datetimenow.strftime("%m/%d/%Y")
time = datetimenow.strftime("%H:%M")

BASE_URL_WEATHER = "http://api.openweathermap.org/data/2.5/weather?appid={0}&zip={1},{2}&units={3}"
final_url_weather = BASE_URL_WEATHER.format(settings["api_key"],settings["zip_code"],settings["country_code"],settings["temp_unit"])
weather_data = requests.get(final_url_weather).json()
# pprint(weather_data['main']['feels_like'])
# pprint(weather_data)
high = weather_data['main']['temp_max']
low = weather_data['main']['temp_min']

BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
final_url_pkmn = BASE_URL_PKMN.format(pkmn_id)
pkmn_data = requests.get(final_url_pkmn).json()
# pprint(pkmn_data)

name = pkmn_data["name"]
id = pkmn_data["id"]
height = str(pkmn_data["height"])
weight = str(pkmn_data["weight"])
types = []
types_data = pkmn_data["types"]
for type in types_data:
    types.append(type["type"]["name"])
first_appearance = ""
if "game_indices" in pkmn_data:
    if len(pkmn_data["game_indices"]) != 0:
        first_appearance = pkmn_data["game_indices"][0]["version"]["name"]

res = {
    "date": date,
    "time": time,
    "high": high,
    "low": low,
    # image of pkmn
    "name": name,
    "id": id,
    "height": height,
    "weight": weight,
    "types": types,
    "fa": first_appearance
}

pprint(res)
