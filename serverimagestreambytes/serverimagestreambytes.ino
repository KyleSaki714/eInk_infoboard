#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64,0x3C for 128x32

const char* ssid = "spl-public";
const char* password = "your_PASSWORD";
const char* url = "https://eink-infoboard.onrender.com";
const char* url_sprite = "https://eink-infoboard.onrender.com/sprite";
const char* url_pkmnInfo = "https://eink-infoboard.onrender.com/pokemonInfo";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int parseXBMWidth(const String& xbmData) {
  int widthIndex = xbmData.indexOf("_width") + 7;
  return xbmData.substring(widthIndex, xbmData.indexOf('\n', widthIndex)).toInt();
}

int parseXBMHeight(const String& xbmData) {
  int heightIndex = xbmData.indexOf("_height") + 8;
  return xbmData.substring(heightIndex, xbmData.indexOf('\n', heightIndex)).toInt();
}

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

void setup() {
  Serial.begin(115200);
  delay(100);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  display.clearDisplay();
  delay(100);

  WiFi.begin(ssid, NULL);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  delay(200);

  HTTPClient http;
  http.begin(url_sprite);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    int len = http.getSize(); // how many bytes the image is
    Serial.print("image size: ");
    Serial.println(len);

    String xbmData = http.getString();
    Serial.println("Downloaded XBM data");

    // Extract width and height from the XBM data
    int width = parseXBMWidth(xbmData);
    int height = parseXBMHeight(xbmData);
    uint8_t* xbmArray = parseXBMArray(xbmData);


    display.clearDisplay();

    display.drawXBitmap(0, 0, xbmArray, width, height, WHITE);
    display.display();

    delete[] xbmArray;  // Free the allocated memory

    Serial.println("Image displayed successfully.");

  } else {
    Serial.println("Failed to download image");
  }
  http.end();

}

void loop() {
  // Nothing to do here
}
