// pokemonoftheday.ino
// Kyle Santos
// Describes the retrieval of display information and images from the server,
// rendering of this information, and hardware functionality.
// Sources used:
// ChatGPT: https://chatgpt.com/share/66fc6ab0-b608-8001-a01c-fa4ad87bd458
// RandomNerdTutorials: https://randomnerdtutorials.com/esp32-https-requests/
// https://randomnerdtutorials.com/esp32-http-get-post-arduino/#http-get-2
// 10-1-24

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64,0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char* ssid = "spl-public";
const char* password = "shaqattack34!";

const char* server = "https://eink-infoboard.onrender.com";
const char* server_pkmnInfo = "https://eink-infoboard.onrender.com/pokemonInfo";
const char* server_sprite = "https://eink-infoboard.onrender.com/sprite";

const char* root_ca = \
"-----BEGIN CERTIFICATE-----\n" \
"MIICCjCCAZGgAwIBAgIQbkepyIuUtui7OyrYorLBmTAKBggqhkjOPQQDAzBHMQsw\n" \
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n" \
"MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n" \
"MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n" \
"Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQA\n" \
"IgNiAATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzu\n" \
"hXyiQHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/l\n" \
"xKvRHYqjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8EBTADAQH/MB0GA1Ud\n" \
"DgQWBBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNnADBkAjBqUFJ0\n" \
"CMRw3J5QdCHojXohw0+WbhXRIjVhLfoIN+4Zba3bssx9BzT1YBkstTTZbyACMANx\n" \
"sbqjYAuG7ZoIapVon+Kz4ZNkfF6Tpt95LY2F45TPI11xzPKwTdb+mciUqXWi4w==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure *client = new WiFiClientSecure;
JSONVar pokeInfo;
String xbmData;

int width;
int height;
uint8_t* xbmArray;

int infoscrollx = SCREEN_WIDTH;

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
    if (bitmapData[i] == '0' && bitmapData[i+1] == 'x') {
      byteCount++;
    }
  }

  // Allocate memory for the array
  uint8_t* xbmArray = new uint8_t[byteCount];
  
  // Parse the hex bytes and fill the array
  int arrayIndex = 0;
  for (int i = 0; i < bitmapData.length(); i++) {
    if (bitmapData[i] == '0' && bitmapData[i+1] == 'x') {
      xbmArray[arrayIndex++] = strtoul(bitmapData.substring(i+2, i+4).c_str(), NULL, 16);
    }
  }

  return xbmArray;
}

void retrievePokemonInfo(HTTPClient* https, WiFiClientSecure* client, JSONVar* pokeInfo) {
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
        Serial.print("Content: ");
        Serial.println(content);
        //Parse JSON
        *pokeInfo = JSON.parse(content);
      }
    }
    else {
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
    }
    else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https->errorToString(httpCode).c_str());
    }
    https->end();
  } else {
    Serial.println("[HTTPS] Failed to retrieve sprite");
  }
}

void retrieveData(WiFiClientSecure* client, JSONVar* pokeInfo, String* xbmData) {
  if(client) {
    // set secure client with certificate
    // client->setCACert(root_ca);
    client->setInsecure();
    //create an HTTPClient instance
    HTTPClient https;

    retrievePokemonInfo(&https, client, pokeInfo);

    retrievePokemonSprite(&https, client, xbmData);

  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }

  client->stop();

  delete client;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  display.display();

  WiFi.begin(ssid, NULL);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  Serial.print("Connected to network with IP Address: ");
  Serial.println(WiFi.localIP());

  retrieveData(client, &pokeInfo, &xbmData);

  // check json
  if (JSON.typeof(pokeInfo) == "undefined") {
    Serial.println("Parsing content json failed");
    return;
  }

  // print JSON
  Serial.println(pokeInfo["firstAppearance"]);
  Serial.println(pokeInfo["height"]);
  Serial.println(pokeInfo["id"]);
  Serial.println(pokeInfo["name"]);
  Serial.println(pokeInfo["types"]);
  Serial.println(pokeInfo["weight"]);

  // Extract width and height from the XBM data
  width = parseXBMWidth(xbmData);
  height = parseXBMHeight(xbmData);
  xbmArray = parseXBMArray(xbmData);

  display.clearDisplay();

  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);
  display.display();

  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);

  Serial.println("Image displayed successfully");
}

void loop() {
  display.clearDisplay();

  display.drawXBitmap(32, 0, xbmArray, width, height, WHITE);

  display.setCursor(infoscrollx, 0);

  Serial.println(pokeInfo["firstAppearance"]);
  Serial.println(pokeInfo["height"]);
  Serial.println(pokeInfo["id"]);
  Serial.println(pokeInfo["name"]);
  Serial.println(pokeInfo["types"]);
  Serial.println(pokeInfo["weight"]);

  // String poketext = pokeInfo["name"] + " #" + pokeInfo["id"] + " " + pokeInfo["firstAppearance"] + " " + pokeInfo["types"] + " " + pokeInfo["height"] + " " + pokeInfo["weight"];
  String poketext = pokeInfo["name"];

  int16_t boundx;
  int16_t boundy;
  uint16_t boundwidth;
  uint16_t boundheight;
  display.getTextBounds(poketext, infoscrollx, 0, &boundx, &boundy, &boundwidth, &boundheight);

  Serial.print("boundx: ");
  Serial.println(boundx);
  Serial.println(boundy);
  Serial.println(boundwidth);
  Serial.println(boundheight);

  if (boundx + boundwidth == 0) {
    infoscrollx = SCREEN_WIDTH;
  } else {
    infoscrollx--;
  }


  display.println(pokeInfo["name"]);
  display.display();
}
