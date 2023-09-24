import random as rand
import pokebase as pb
import requests
from pprint import pprint

id = rand.randint(0, 1017)

BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
final_url_pkmn = BASE_URL_PKMN.format(id)
pkmn_data = requests.get(final_url_pkmn).json()
#pprint(pkmn_data)

print(pkmn_data["sprites"]["front_default"])
print(pkmn_data["name"])
print(pkmn_data["id"])
print("Height " + str(pkmn_data["height"]) + "dm")
print("Weight " + str(pkmn_data["weight"]) + "hg")
types = pkmn_data["types"]
for type in types:
    print(type["type"]["name"])

if "game_indices" in pkmn_data:
    if len(pkmn_data["game_indices"]) != 0:
        print("First Appearance: Pkmn " + pkmn_data["game_indices"][0]["version"]["name"])

