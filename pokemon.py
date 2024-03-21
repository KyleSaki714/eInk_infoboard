import random as rand
import requests
import urllib.request
from PIL import ImageOps
from PIL import Image
from io import BytesIO
from pprint import pprint
# import xbm
import io
# import pyexiv2
import cairo

id = rand.randint(0, 1017)
# id = 483
# id = 506/
# Favorites
# 250

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

BASE_URL_CHARACTERISTIC = ""

pokeApi_data = requests.get(final_url_pkmn).json()
# pprint(pkmn_data)

pokeInfo = dict()
pokeInfo['name'] = pokeApi_data["name"]
pokeInfo['id'] = pokeApi_data["id"]
pokeInfo['firstAppearance'] = getFirstAppearance(pokeApi_data);
pokeInfo['height'] = getHeight(pokeApi_data)
pokeInfo['weight'] = getWeight(pokeApi_data)
pokeInfo['types'] = getTypes(pokeApi_data)

print(pokeInfo["sprite"])
print(pokeInfo['name'])
print(pokeApi_data['id'])
print(pokeInfo['firstAppearance'])
# print("Height " + str(pokeApi_data["height"]) + "dm")
print(pokeInfo["height"])
# print("Weight " + str(pokeApi_data["weight"]) + "hg")
print(pokeInfo['weight'])
print(pokeInfo['types'])

# sprite = requests.get(pkmn_data["sprites"]["front_default"])
# print(pkmn_data["sprites"]["front_default"])

urllib.request.urlretrieve(pokeApi_data["sprites"]["front_default"], "sprite.png")
im = Image.open("sprite.png")
im.size  # (364, 471)
im.getbbox()  # (64, 89, 278, 267)
im2 = im.crop(im.getbbox())
# im2 = ImageOps.solarize(im2, 128)
im2.size  # (214, 178)
# im2.resize((128, 64))
im2.thumbnail((48,48), 2)
im2.save("sprite_resized_unedited.png")
im2 = im2.convert('1')

try:
    im2.save("sprite.png")
except:
    im2.save("sprite.bmp", "BMP")
im2.save("sprite.xbm", "XBM")

pokeInfo["sprite"] = Image.open("sprite.xbm")

# xmp_obj = xmp.XMP()
# xmp_obj.add_data(im2.tobytes())

# with io.open("sprite.xmp", "wb") as f:
#     f.write(xmp_obj.toString())

# print(bytes.hex())

# spritex = pyexiv2.Image("sprite.png")
# spritex_xmp = spritex.read_raw_xmp()

# with io.open("sprite.xmp", "wb") as f:
#     f.write(spritex_xmp)


# image = cairo.ImageSurface.create_from_png("sprite.png")

# # Create a bitmap surface
# bitmap = cairo.ImageSurface.create_from_x11(image.get_width(), image.get_height())

# # Draw the image to the bitmap surface
# cairo.Context(bitmap).set_source_surface(image)
# cairo.Context(bitmap).paint()

# # Save the bitmap surface
# bitmap.write_to_png("image.xpm")