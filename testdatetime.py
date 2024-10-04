from datetime import datetime
from datetime import timedelta
import os.path
import json

# lastChecked = datetime.today().date() - timedelta(days = 1)
# print(lastChecked)

# if (datetime.today().date() > lastChecked):
#   print("new pkmn")
# else:
#   print("fdsafds")
  
print(os.path.isfile("pokeinfo.json"))
file = open("pokeinfo.json", "r")
data = json.load(file)
print (str(data))
print(data["id"])