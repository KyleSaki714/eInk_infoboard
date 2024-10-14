from flask import Flask, redirect, jsonify, send_from_directory
import random as rand
import requests
import urllib.request
from PIL import ImageOps
from PIL import Image
from io import BytesIO
import io
import os.path
import json
from datetime import datetime

FILE_CURRENTDATE = "currentdate"
FILE_POKEINFO = "pokeinfo.json"

# using the given pokeapi data, 
# gets the game(s) the pokemon first appeared in.
# returns a string that describes this.
def getFirstAppearance(pokeApi_data):    
    text = ''
    spritesVersions = pokeApi_data["sprites"]["versions"]
    for gen in spritesVersions:
        for game in spritesVersions[gen]:
            if (spritesVersions[gen][game]["front_default"] != None):
                text += gen
                text = text.replace("eration-"," ",2)
                text += ": "
                for firstAppearanceGenGame in spritesVersions[gen]:
                    text += firstAppearanceGenGame
                    text += " "
                return text

# using the given pokeapi data, 
# gets the type(s) of the pokemon.
# returns a comma separated string of these values.
def getTypes(pokeApi_data):
    text = ''
    types = pokeApi_data["types"]
    if (len(types) > 0):
        text += types[0]["type"]["name"]
        if (len(types) > 1):
            for n in range(1, len(types)):
                text += ", "
                text += types[n]["type"]["name"]
    return text

# using the given pokeapi data, 
# Converts the height of the pokemon
# from decimeters to feet and inches. returns this value. 
def getHeight(pokeApi_data):
    num = pokeApi_data["height"]
    dmToFeet = 0.3280839895
    return str(round(num * dmToFeet, 2)) + " ft"

# using the given pokeapi data, 
# Converts the weight of this pokemon
# from hectograms to pounds. returns this value.
def getWeight(pokeApi_data):
    num = pokeApi_data["weight"]
    hgToPounds = 0.2204622622
    return str(round(num * hgToPounds, 2)) + " lbs"

# retrieves sprite image from pokeapi data,
# then converts into 1-bit displayable on ssd1306 displays .
# saves the xbm image to "sprite.xbm"
def generateSprite(pokeApi_data):
    # get sprite from api
    urllib.request.urlretrieve(pokeApi_data["sprites"]["front_default"], "sprite.png")
    im = Image.open("sprite.png")

    # crop to bounding box
    im2 = im.crop(im.getbbox()) 

    # resize sprite using thumbnail function size to 48x48 px, and use Bilinear resampling https://pillow.readthedocs.io/en/stable/reference/Image.html#resampling-filters
    im2 = im2.resize((64, 64), 2)

    # convert image to 1 bit (b/w)
    im2 = im2.convert('1')

    # export to xbm for esp32
    im2.save("sprite.xbm", "XBM")

# gathers new pokemon data using the 
# given pokemon id, using the PokeAPI.
# returns a concise dictionary of pokemon info to display.
def retrievePkmnData(id):
        
    BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
    final_url_pkmn = BASE_URL_PKMN.format(id)

    pokeApi_data = requests.get(final_url_pkmn).json()

    pokeInfo = dict()
    pokeInfo['name'] = pokeApi_data["name"]
    pokeInfo['id'] = pokeApi_data["id"]
    pokeInfo['firstAppearance'] = getFirstAppearance(pokeApi_data)
    pokeInfo['height'] = getHeight(pokeApi_data)
    pokeInfo['weight'] = getWeight(pokeApi_data)
    pokeInfo['types'] = getTypes(pokeApi_data)
    pokeInfo['timestamp'] = int(datetime.now().timestamp())
    generateSprite(pokeApi_data)
    return pokeInfo

# generates a new id for a pokemon.
# id will work with PokeAPI.
# returns the id.
def genPkmnId():
    return rand.randint(0, 1017)

app = Flask(__name__)

lastKnownDay = datetime.today().date()

# on server spin up, try to read today's date from file "currentdate"
try:
    with open(FILE_CURRENTDATE, "r") as file:
        lines = file.readlines()
        date_format = "%Y-%m-%d"
        dateobj = datetime.strptime(lines[0], date_format)
        lastKnownDay = dateobj.date()
except:
    lastKnownDay = datetime.today().date()
    f = open("currentdate", "w")
    d = f.write(str(lastKnownDay))

pokeInfo = None

# on server spin up, try to read pokeinfo from file "pokeinfo.json"
try:
    # open existing pkmn file
    with open(FILE_POKEINFO, "r") as file:
        pokeInfo = json.load(file)
except:
    # if not a successful read, generate a new pokemon and
    # save new pokeinfo.json to disk
    pokemonId = genPkmnId()
    pokeInfo = retrievePkmnData(pokemonId)
    
    with open(FILE_POKEINFO, "w") as file:
        json.dump(pokeInfo, file, indent=4)

print("lastKnownDay: ", lastKnownDay)
print("pokeInfo: ", pokeInfo)

# checks if a new pokemon must be generated for a new day.
# if so, retrieves new pokemon data from PokeAPI. else if 
# not a new day, does nothing as pokeinfo.json should already be on disk.
def checkNewDayNewPkmn():
    global lastKnownDay
    global pokeInfo
    currentDay = datetime.today().date()
    if (currentDay > lastKnownDay):
        pokeInfo = retrievePkmnData(genPkmnId())
        with open(FILE_POKEINFO, "w") as file:
            json.dump(pokeInfo, file, indent=4)
        print("new day: new pokemon generated")
        lastKnownDay = currentDay
        
        f = open("currentdate", "w")
        d = f.write(str(lastKnownDay))
    
# root endpoint that when visited, checks if it is a new day.
# if so, retrieves a new pokemon from PokeAPI.
@app.get('/')
def index():
    global lastKnownDay
    global pokeInfo
    checkNewDayNewPkmn()
    res = "<h1>Today's Pokemon: " + pokeInfo['name'] + "</h1>"
    if (datetime.today().date() == lastKnownDay):
        res += "<p>come back tomorrow for a new pokemon!</p>"
    res += "<p>Please refer to the endpoints below</p><p>/pokeInfo: returns pokeInfo.json</p><p>/sprite: returns sprite.xbm</p>"
    return res

# Endpoint that serves a json file of pokemon info for
# the device to display.
@app.get('/pokemonInfo')
def pokemonInfo():
    checkNewDayNewPkmn()
    return send_from_directory("", FILE_POKEINFO)

# Endpoint that serves an xbm image file for the device 
# to read and display. 
@app.get('/sprite')
def sprite():
    return send_from_directory("", "sprite.xbm")

# Endpoint that forces the server to generate 
# a new pokemon for the day.
# Redirects to endpoint '/pokemonInfo' to see the
# new pokemon.
@app.get('/newPokemon')
def newPokemon():
    global pokeinfo
    pokemonId = genPkmnId()
    pokeInfo = retrievePkmnData(pokemonId)
    
    with open(FILE_POKEINFO, "w") as file:
        json.dump(pokeInfo, file, indent=4)
        print("success new pkmn")
    
    return redirect('pokemonInfo')

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=10000)