import random as rand
import requests
import urllib.request
from PIL import ImageOps
from PIL import Image
from io import BytesIO
from pprint import pprint
import io
import json

id = rand.randint(0, 1017)

def getFirstAppearance(pokeApi_data):

    # if "game_indices" in pokeApi_data:
    #     if len(pokeApi_data["game_indices"]) != 0:
    #         # print("First Appearance: Pkmn " + pokeApi_data["game_indices"][0]["version"]["name"])
    #         return pokeApi_data["game_indices"][0]["version"]["name"]
    
    text = ''
    spritesVersions = pokeApi_data["sprites"]["versions"]
    for gen in spritesVersions:
        for game in spritesVersions[gen]:
            if (spritesVersions[gen][game]["front_default"] != None):
                # pokeInfo['firstAppearance'] += "first appearance: ", gen
                text += gen
                text = text.replace("eration-"," ",2)
                text += ": "
                # print("(games: ",end="")
                for firstAppearanceGenGame in spritesVersions[gen]:
                    # print(firstAppearanceGenGame,", ", end="")
                    text += firstAppearanceGenGame
                    text += " "
                # print()
                return text

def getTypes(pokeApi_data):
    text = ''
    types = pokeApi_data["types"]
    if (len(types) > 0):
        text += types[0]["type"]["name"]
        if (len(types) > 1):
            for n in range(1, len(types)):
                text += ", "
                text += types[n]["type"]["name"]
    # print(type["type"]["name"],", ",end="")
        
    return text

# Converts the height from the given API Data
# from decimeters to feet and inches. 
def getHeight(pokeApi_data):
    num = pokeApi_data["height"]
    dmToFeet = 0.3280839895
    return str(round(num * dmToFeet, 2)) + " ft"

# Converts the weight from the given API data
# from hectograms to pounds
def getWeight(pokeApi_data):
    num = pokeApi_data["weight"]
    hgToPounds = 0.2204622622
    return str(round(num * hgToPounds, 2)) + " lbs"

BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
final_url_pkmn = BASE_URL_PKMN.format(id)

pokeApi_data = requests.get(final_url_pkmn).json()

pokeInfo = dict()
pokeInfo['name'] = pokeApi_data["name"]
pokeInfo['id'] = pokeApi_data["id"]
pokeInfo['firstAppearance'] = getFirstAppearance(pokeApi_data);
pokeInfo['height'] = getHeight(pokeApi_data)
pokeInfo['weight'] = getWeight(pokeApi_data)
pokeInfo['types'] = getTypes(pokeApi_data)

print(pokeInfo['name'])
print(pokeApi_data['id'])
print(pokeInfo['firstAppearance'])
print(pokeInfo["height"])
print(pokeInfo['weight'])
print(pokeInfo['types'])

# get sprite from api
urllib.request.urlretrieve(pokeApi_data["sprites"]["front_default"], "sprite.png")
im = Image.open("sprite.png")

# crop to bounding box
im2 = im.crop(im.getbbox()) 

# resize sprite using thumbnail function size to 48x48 px, and use Bilinear resampling https://pillow.readthedocs.io/en/stable/reference/Image.html#resampling-filters
im2 = im2.resize((64, 64), 2)
im2.save("sprite_resized_unedited.png")

# convert image to 1 bit (b/w)
im2 = im2.convert('1')

# export to png for debugging purposes
try:
    im2.save("sprite.png")
except:
    im2.save("sprite.bmp", "BMP")

# export to xbm for esp32
im2.save("sprite.xbm", "XBM")

xbm = open("sprite.xbm", "r")
xbmTxt = xbm.read()
pokeInfo["sprite"] = xbmTxt
xbm.close()

print(pokeInfo)
with open("pokeinfo.json" , "w") as write:
    json.dump(pokeInfo, write)