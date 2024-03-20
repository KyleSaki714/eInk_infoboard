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
# Favorites
# 250


BASE_URL_PKMN = "https://pokeapi.co/api/v2/pokemon/{0}"
final_url_pkmn = BASE_URL_PKMN.format(id)

BASE_URL_CHARACTERISTIC = ""

pkmn_data = requests.get(final_url_pkmn).json()
# pprint(pkmn_data)

print(pkmn_data["sprites"]["front_default"])
print(pkmn_data["name"])
print(pkmn_data["id"])
spritesVersions = pkmn_data["sprites"]["versions"]
done = False
for gen in spritesVersions:
    if done == True: break
    for game in spritesVersions[gen]:
        if (spritesVersions[gen][game]["front_default"] != None):
            print("first appearance: ",gen);
            print("(games: ",end="")
            for firstAppearanceGenGame in spritesVersions[gen]:
                print(firstAppearanceGenGame,", ", end="")
            done = True
            print()
            break

# print("Height " + str(pkmn_data["height"]) + "dm")
# print("Weight " + str(pkmn_data["weight"]) + "hg")
types = pkmn_data["types"]
for type in types:
    print(type["type"]["name"],", ",end="")
print()
if "game_indices" in pkmn_data:
    if len(pkmn_data["game_indices"]) != 0:
        print("First Appearance: Pkmn " + pkmn_data["game_indices"][0]["version"]["name"])

# sprite = requests.get(pkmn_data["sprites"]["front_default"])
# print(pkmn_data["sprites"]["front_default"])
urllib.request.urlretrieve(pkmn_data["sprites"]["front_default"], "sprite.png")
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