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
#include <SPI.h>
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
#define FILENAME_SPRITE "/sprite.xbm"

// const char* ssid = "spl-public";
const char* ssid = "Samsung Galaxy Note9_0270";
// const char* password = "shaqattack34!";
const char* password = "02927731";

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

void listAllFiles(){
 
  File root = SPIFFS.open("/");
 
  File file = root.openNextFile();
 
  while(file){
 
      Serial.print("FILE: ");
      Serial.println(file.name());
 
      file = root.openNextFile();
  }
 
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

  // initialize SPIFFS
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  listAllFiles();

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
    pFile.close();
  }

  Serial.println(pokeInfo["firstAppearance"].as<String>());
  Serial.println(pokeInfo["height"].as<String>());
  Serial.println(pokeInfo["id"].as<int>());
  Serial.println(pokeInfo["name"].as<String>());
  Serial.println(pokeInfo["types"].as<String>());
  Serial.println(pokeInfo["weight"].as<String>());
  return;

  // Clear the buffer
  display.clearDisplay();
  display.display();

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.print("Connected to network with IP Address: ");
  Serial.println(WiFi.localIP());

  retrieveData(client, &pokeInfo, &xbmData);

  // TODO: check json is valid
  // if (JSON.typeof(pokeInfo) == "undefined") {
  //   Serial.println("Parsing content json failed");
  //   return;
  // }

  // cache pokeInfo
  if (!SPIFFS.exists(FILENAME_POKEINFO)) {
    Serial.println("pokeInfo.json does not exist, creating file...");
    File createFile = SPIFFS.open(FILENAME_POKEINFO, FILE_WRITE);
    if (!createFile) {
      Serial.println("Failed to create file");
      return;
    }

    serializeJson(pokeInfo, createFile);
    createFile.close();
  }


  // cache xbm 

  // Extract width and height from the XBM data
  width = parseXBMWidth(xbmData);
  height = parseXBMHeight(xbmData);
  xbmArray = parseXBMArray(xbmData);

  display.clearDisplay();

  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.display();

  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(true);

  Serial.println("Image displayed successfully");
}

void loop() {
  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  String pokeText = pokeInfo["name"];
  pokeText.replace("\"", "");
  display.println(pokeText);
  display.display();
  delay(infoFadeDelay);

  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.print("#");
  display.println((int) pokeInfo["id"]);
  display.display();
  delay(infoFadeDelay);

  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.print("types: ");
  String pokeText2 = pokeInfo["types"];
  pokeText.replace("\"", "");
  display.println(pokeText2);
  display.display();
  delay(infoFadeDelay);

  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.print("first appearance: ");
  String pokeText3 = pokeInfo["firstAppearance"];
  pokeText.replace("\"", "");
  display.println(pokeText3);
  display.display();
  delay(infoFadeDelay);

  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.print("height: ");
  String pokeText4 = pokeInfo["height"];
  pokeText.replace("\"", "");
  display.println(pokeText4);
  display.display();
  delay(infoFadeDelay);

  display.clearDisplay();
  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.setCursor(0, 0);
  display.print("weight: ");
  String pokeText5 = pokeInfo["weight"];
  pokeText.replace("\"", "");
  display.println(pokeText5);
  display.display();
  delay(infoFadeDelay);
}
