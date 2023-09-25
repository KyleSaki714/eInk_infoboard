import random as rand
import pokebase as pb
import requests
import urllib.request
from PIL import ImageOps
from PIL import Image
from io import BytesIO
from pprint import pprint
# import xbm
import io
import pyexiv2

id = rand.randint(0, 1017)
# id = 483

BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
final_url_pkmn = BASE_URL_PKMN.format(id)
# pkmn_data = requests.get(final_url_pkmn).json()
#pprint(pkmn_data)

# print(pkmn_data["sprites"]["front_default"])
# print(pkmn_data["name"])
# print(pkmn_data["id"])
# print("Height " + str(pkmn_data["height"]) + "dm")
# print("Weight " + str(pkmn_data["weight"]) + "hg")
# types = pkmn_data["types"]
# for type in types:
#     print(type["type"]["name"])

# if "game_indices" in pkmn_data:
#     if len(pkmn_data["game_indices"]) != 0:
#         print("First Appearance: Pkmn " + pkmn_data["game_indices"][0]["version"]["name"])

# sprite = requests.get(pkmn_data["sprites"]["front_default"])
# urllib.request.urlretrieve(pkmn_data["sprites"]["front_default"], "sprite.png")
im = Image.open("483.png")
im.size  # (364, 471)
im.getbbox()  # (64, 89, 278, 267)
im2 = im.crop(im.getbbox())
# im2 = ImageOps.solarize(im2, 128)
im2.save("sprite_unedited.png")
im2.size  # (214, 178)
# im2.resize((128, 64))
im2.thumbnail((48,48), 2)
im2 = im2.convert('1')
im2.save("sprite.png")
im2.save("sprite.bmp", "BMP")


# xmp_obj = xmp.XMP()
# xmp_obj.add_data(im2.tobytes())

# with io.open("sprite.xmp", "wb") as f:
#     f.write(xmp_obj.toString())

# print(bytes.hex())

# spritex = pyexiv2.Image("sprite.png")
# spritex_xmp = spritex.read_raw_xmp()

# with io.open("sprite.xmp", "wb") as f:
#     f.write(spritex_xmp)


