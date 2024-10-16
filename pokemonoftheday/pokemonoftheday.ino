// pokemonoftheday.ino
// Kyle Santos
// Describes the retrieval of display information and images from the server,
// rendering of this information, and hardware functionality.
// Sources used:
// ChatGPT: https://chatgpt.com/share/66fc6ab0-b608-8001-a01c-fa4ad87bd458
// RandomNerdTutorials: https://randomnerdtutorials.com/esp32-https-requests/
// https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-get-2
// 10-1-24

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <List.hpp>
#include <SPI.h>
#include <TimeLib.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>
#include "FS.h"
#include "SPIFFS.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D  ///< See datasheet for Address; 0x3D for 128x64,0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define FORMAT_SPIFFS_IF_FAILED true
#define FILENAME_POKEINFO "/pokeInfo.json"
#define FILENAME_SPRITE "/sprite.txt"
#define DAY_SECONDS 86400

// const char* ssid = "spl-public";
const char* ssid = "Samsung Galaxy Note9_0270";
// const char* ssid = "University of Washington";
// const char* password = "shaqattack34!";
const char* password = "02927731";

const int CONNECTWIFI_TIMELIMIT = 3000;
const char* server = "https://eink-infoboard.onrender.com";
const char* server_pkmnInfo = "https://eink-infoboard.onrender.com/pokemonInfo";
const char* server_sprite = "https://eink-infoboard.onrender.com/sprite";

const char* root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "MIICCjCCAZGgAwIBAgIQbkepyIuUtui7OyrYorLBmTAKBggqhkjOPQQDAzBHMQsw\n"
  "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
  "MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
  "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
  "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQA\n"
  "IgNiAATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzu\n"
  "hXyiQHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/l\n"
  "xKvRHYqjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud\n"
  "DgQWBBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNnADBkAjBqUFJ0\n"
  "CMRw3J5QdCHojXohw0+WbhXRIjVhLfoIN+4Zba3bssx9BzT1YBkstTTZbyACMANx\n"
  "sbqjYAuG7ZoIapVon+Kz4ZNkfF6Tpt95LY2F45TPI11xzPKwTdb+mciUqXWi4w==\n"
  "-----END CERTIFICATE-----\n";

WiFiClientSecure* client = new WiFiClientSecure;
JsonDocument pokeInfo;
String xbmData;

int width;
int height;
uint8_t* xbmArray;

int infoscrollx = SCREEN_WIDTH;
const int infoFadeDelay = 2500;
List<String> pokeTextList;
int currPokeText = 0;

const int PIN_BUTTON = 13;

// ChatGPT function
int parseXBMWidth(const String& xbmData) {
  int widthIndex = xbmData.indexOf("_width") + 7;
  return xbmData.substring(widthIndex, xbmData.indexOf('\n', widthIndex)).toInt();
}

// ChatGPT function
int parseXBMHeight(const String& xbmData) {
  int heightIndex = xbmData.indexOf("_height") + 8;
  return xbmData.substring(heightIndex, xbmData.indexOf('\n', heightIndex)).toInt();
}

// ChatGPT function
uint8_t* parseXBMArray(const String& xbmData) {
  int dataStart = xbmData.indexOf("{") + 1;
  int dataEnd = xbmData.indexOf("}");
  String bitmapData = xbmData.substring(dataStart, dataEnd);

  // Count the number of hex bytes in the array
  int byteCount = 0;
  for (int i = 0; i < bitmapData.length(); i++) {
    if (bitmapData[i] == '0' && bitmapData[i + 1] == 'x') {
      byteCount++;
    }
  }

  // Allocate memory for the array
  uint8_t* xbmArray = new uint8_t[byteCount];

  // Parse the hex bytes and fill the array
  int arrayIndex = 0;
  for (int i = 0; i < bitmapData.length(); i++) {
    if (bitmapData[i] == '0' && bitmapData[i + 1] == 'x') {
      xbmArray[arrayIndex++] = strtoul(bitmapData.substring(i + 2, i + 4).c_str(), NULL, 16);
    }
  }

  return xbmArray;
}

void retrievePokemonInfo(HTTPClient* https, WiFiClientSecure* client, JsonDocument* pokeInfo) {
  //Initializing an HTTPS communication using the secure client
  Serial.print("[HTTPS] begin...\n");
  if (https->begin(*client, server_pkmnInfo)) {  // HTTPS
    Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https->GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        // print server response payload
        String content = https->getString();
        Serial.print("pokeInfo: ");
        Serial.println(content);

        JsonDocument doc;
        deserializeJson(doc, content);

        *pokeInfo = doc;
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https->errorToString(httpCode).c_str());
    }
    https->end();
  } else {
    Serial.println("[HTTPS] Failed to retrieve pokemonInfo");
  }
}

void retrievePokemonSprite(HTTPClient* https, WiFiClientSecure* client, String* xbmData) {
  // retrieve sprite
  if (https->begin(*client, server_sprite)) {  // HTTPS
    Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https->GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      // file found at server
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        *xbmData = https->getString();
        Serial.println("Downloaded sprite XBM data");
      }
    } else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https->errorToString(httpCode).c_str());
    }
    https->end();
  } else {
    Serial.println("[HTTPS] Failed to retrieve sprite");
  }
}

void retrieveData(WiFiClientSecure* client, JsonDocument* pokeInfo, String* xbmData) {
  if (client) {
    // set secure client with certificate
    // client->setCACert(root_ca);
    client->setInsecure();
    //create an HTTPClient instance
    HTTPClient https;

    retrievePokemonInfo(&https, client, pokeInfo);

    retrievePokemonSprite(&https, client, xbmData);

  } else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  client->stop();

  delete client;
}

// Displays the pokemon using the provided info.
void displayPokemon(JsonDocument* pokeInfo, String* xbmData) {
  
  // Extract width and height from the XBM data
  width = parseXBMWidth(*xbmData);
  height = parseXBMHeight(*xbmData);
  xbmArray = parseXBMArray(*xbmData);

  display.clearDisplay();

  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.display();

  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(true);

  Serial.println("Image displayed successfully");
  // TODO delete xbmArray;
}

void listAllFiles(){
 
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();
 
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());
 
      file = root.openNextFile();
  }
 
}

void checkCache() {
  if (SPIFFS.exists(FILENAME_POKEINFO)) {
    File pFile = SPIFFS.open(FILENAME_POKEINFO, FILE_READ);
    if (!pFile) {
      Serial.println("fail to open file");
      return;
    }
    DeserializationError error = deserializeJson(pokeInfo, pFile);
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      pFile.close();
      return;
    }
    Serial.println("read in cached pokeInfo.json file:");
    pFile.close();
  }

  if (SPIFFS.exists(FILENAME_SPRITE)) {
    File xFile = SPIFFS.open(FILENAME_SPRITE, FILE_READ);
    if (!xFile) {
      Serial.println("fail to open file");
      return;
    }
    size_t xFileLen = xFile.available();
    uint8_t xFileBuf[xFileLen];
    xFile.read(xFileBuf, xFileLen);
    // xbmData = (String) xFile.read();
    xbmData = (char*) xFileBuf;
        // Serial.write(xFile.read());
    
    Serial.println("read in cached xbm file.");
    // delete[] xFileBuf;
    xFile.close();
  }
}

int tryConnectWifi() {
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    // if can't connect to wifi, return from setup() and go to loop()
    if ((millis() - startTime) > CONNECTWIFI_TIMELIMIT) {
      return -1;
    }
    Serial.print(".");
    delay(1000);
  }
  return 0;
}

// check if newly retrieved pokemon data can and should be used.
// returns 0 if new pokeinfo will be used. otherwise 1 and not be used.
int checkPokemonData(JsonDocument newPokeInfo, JsonDocument oldPokeinfo) {
  // TOOD: check if JSON is valid

  // using the timestamp, check if it's a new day
  time_t newTime = newPokeInfo["timestamp"].as<long>();
  time_t oldTime = pokeInfo["timestamp"].as<long>();
  return (newTime <= oldTime); // older data, discard
}

// writes the current pokeInfo.json to file. overwrites any existing cache.
int cachePokeInfo(JsonDocument* pokeInfo) {
  Serial.println("pokeInfo.json does not exist, creating file...");
  File createPokeInfoFile = SPIFFS.open(FILENAME_POKEINFO, FILE_WRITE);
  if (!createPokeInfoFile) {
    Serial.println("Failed to create file");
    return 1;
  }

  serializeJson(*pokeInfo, createPokeInfoFile);
  createPokeInfoFile.close();
  return 0;
}

// writes the current sprite to file. overwrites any existing cache.
int cacheSprite(String* xbmData) {
  Serial.println("sprite.xbm does not exist, creating file...");
  File createXbmFile = SPIFFS.open(FILENAME_SPRITE, FILE_WRITE);
  if (!createXbmFile) {
    Serial.println("Failed to create file");
    return 1;
  }

  if(createXbmFile.println(*xbmData)){
    Serial.println("xbm file written");
  } else {
    Serial.println("xbm write failed");
    return 1;
  }

  createXbmFile.close();
  return 0;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // initialize button
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // initialize SPIFFS
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  listAllFiles();

  // check if cached pokeInfo.json and sprite.txt are available
  checkCache();

  // Clear the buffer
  display.clearDisplay();
  display.display();

  // TODO: display cached pokemonInfo and image
  displayPokemon(&pokeInfo, &xbmData);

  // try to connect to wifi
  if (tryConnectWifi()) {
    Serial.println("Failed to connect to WiFi, using cached Pokemon info...");
    return;
  }
  
  Serial.print("Connected to network with IP Address: ");
  Serial.println(WiFi.localIP());

  // retrieve new pokemon data from server
  JsonDocument newPokeInfo;
  String newXbmData;
  retrieveData(client, &newPokeInfo, &newXbmData);
  Serial.println("sup");
  pokeTextList.add(pokeInfo["name"].as<String>());
  String pokeText = "#";
  pokeText += pokeInfo["id"].as<String>(); 
  pokeTextList.add(pokeText);
  pokeText = "types: ";
  pokeText += pokeInfo["types"].as<String>();
  pokeTextList.add(pokeText);
  pokeText = "first appearance: ";
  pokeText += pokeInfo["firstAppearance"].as<String>();
  pokeTextList.add(pokeText);
  pokeText = "height: ";
  pokeText += pokeInfo["height"].as<String>();
  pokeTextList.add(pokeText);
  pokeText = "weight: ";
  pokeText += pokeInfo["weight"].as<String>();
  pokeTextList.add(pokeText);
  Serial.println("sup2");

  if (checkPokemonData(newPokeInfo, pokeInfo)) {
    return;
  }

  Serial.println("1 day passed since last pokemon info retrieval, OR new pokemon generated on server, getting new pkmn..");
  pokeInfo = newPokeInfo;
  xbmData = newXbmData;

  // cache pokeInfo and sprite
  cachePokeInfo(&pokeInfo);
  cacheSprite(&xbmData);


  // Serial.println(pokeInfo["firstAppearance"].as<String>());
  // Serial.println(pokeInfo["height"].as<String>());
  // Serial.println(pokeInfo["id"].as<int>());
  // Serial.println(pokeInfo["name"].as<String>());
  // Serial.println(pokeInfo["types"].as<String>());
  // Serial.println(pokeInfo["weight"].as<String>());
  // Serial.println(pokeInfo["timestamp"].as<int>());

  Serial.println(pokeTextList[0]);
  Serial.println(pokeTextList[1]);
  Serial.println(pokeTextList[2]);
  Serial.println(pokeTextList[3]);
  Serial.println(pokeTextList[4]);
  Serial.println(pokeTextList[5]);
  Serial.println(pokeTextList[6]);
  Serial.println(pokeTextList[7]);

  displayPokemon(&pokeInfo, &xbmData);
}

void loop() {
  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.println(pokeTextList[(currPokeText % pokeTextList.getSize())]);
  display.display();
  delay(infoFadeDelay);
  currPokeText++;

  int btnval = digitalRead(PIN_BUTTON);
  Serial.println(btnval);
}
