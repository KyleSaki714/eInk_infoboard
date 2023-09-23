import time
import requests
from pprint import pprint

settings = {
    'api_key':'94d9fc275a89afcc4bff8278e5b04ae6',
    'zip_code':'98118',
    'country_code':'us',
    'temp_unit':'imperial'} #unit can be metric, imperial, or kelvin

BASE_URL = "http://api.openweathermap.org/data/2.5/weather?appid={0}&zip={1},{2}&units={3}"

while True:
    final_url = BASE_URL.format(settings["api_key"],settings["zip_code"],settings["country_code"],settings["temp_unit"])
    weather_data = requests.get(final_url).json()
    pprint(weather_data['main']['feels_like'])
    time.sleep(20) #get new data every 20 seconds