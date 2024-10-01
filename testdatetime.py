from datetime import datetime
from datetime import timedelta

lastChecked = datetime.today().date() - timedelta(days = 1)
print(lastChecked)

if (datetime.today().date() > lastChecked):
  print("new pkmn")
else:
  print("fdsafds")