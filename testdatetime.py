from datetime import datetime
from datetime import timedelta
import os.path

# lastChecked = datetime.today().date() - timedelta(days = 1)
# print(lastChecked)

# if (datetime.today().date() > lastChecked):
#   print("new pkmn")
# else:
#   print("fdsafds")
  
print(os.path.isfile("sprite.xbm"))
file = open("pkmnidtest.txt", "r")
lines = file.readlines()
print(lines[0])