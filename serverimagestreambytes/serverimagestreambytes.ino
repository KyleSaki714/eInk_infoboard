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
const char* bitmapUrl = "https://eink-infoboard.onrender.com/sprite";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

  delay(500);

  HTTPClient http;
  http.begin(bitmapUrl);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    int len = http.getSize();
    Serial.print("imagesize ");
    Serial.println(len);
    uint8_t bitmap[SCREEN_HEIGHT * SCREEN_HEIGHT / 8] PROGMEM;  // 1-bit per pixel, 128x64 display size
    WiFiClient* stream = http.getStreamPtr();
    stream->readBytes(bitmap, len);

    // Display the downloaded bitmap
    display.clearDisplay();
    display.drawBitmap(0, 0, bitmap, SCREEN_HEIGHT, SCREEN_HEIGHT, WHITE);
    display.display();
  } else {
    Serial.println("Failed to download image");
  }
  http.end();
}

void loop() {
  // Nothing to do here
}
