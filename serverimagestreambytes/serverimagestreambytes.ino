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

// Function to flip a monochrome bitmap horizontally
void flipHorizontal(uint8_t* bitmap, int width, int height) {
  int bytesPerRow = width / 8;

  // Loop through each row
  for (int y = 0; y < height; y++) {
    uint8_t* rowStart = bitmap + (y * bytesPerRow);  // Pointer to the start of the row
    // Swap bytes horizontally within the row
    for (int x = 0; x < bytesPerRow / 2; x++) {
      // Swap bytes at position x and (bytesPerRow - x - 1)
      uint8_t temp = rowStart[x];
      rowStart[x] = reverseBits(rowStart[bytesPerRow - x - 1]);
      rowStart[bytesPerRow - x - 1] = reverseBits(temp);
    }

    // If the number of bytes per row is odd, reverse the middle byte
    if (bytesPerRow % 2 != 0) {
      rowStart[bytesPerRow / 2] = reverseBits(rowStart[bytesPerRow / 2]);
    }
  }
}

// Helper function to reverse bits in a byte
uint8_t reverseBits(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void flipVertical(uint8_t* bitmap, int width, int height) {
  for (int y = 0; y < height / 2; y++) {
    for (int x = 0; x < width / 8; x++) {
      uint8_t temp = bitmap[y * (width / 8) + x];
      bitmap[y * (width / 8) + x] = bitmap[(height - 1 - y) * (width / 8) + x];
      bitmap[(height - 1 - y) * (width / 8) + x] = temp;
    }
  }
}

uint8_t reverseByte(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

void reverseBitmap(uint8_t* bitmap, int len) {
  for (int i = 0; i < len; i++) {
    bitmap[i] = reverseByte(bitmap[i]);
  }
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

  delay(500);

  HTTPClient http;
  http.begin(bitmapUrl);
  int httpCode = http.GET();
  
  if (httpCode == 200) {
    int len = http.getSize(); // how many bytes the image is
    Serial.print("image size: ");
    Serial.println(len);

    // dynamically allocate array for image size
    // uint8_t bitmap[SCREEN_HEIGHT * SCREEN_HEIGHT / 8] PROGMEM;
    uint8_t* bitmap = new uint8_t[len];

    WiFiClient* stream = http.getStreamPtr();
    size_t bytesRead = stream->readBytes(bitmap, len);

    if (bytesRead == len) {
      // Fix bit order and flip the image as needed
      // reverseBitmap(bitmap, len);
      // flipVertical(bitmap, 64, 64);  // Flip vertically to correct the orientation

      display.clearDisplay();
      
      // Center the image if it is 64x64
      // int x = (SCREEN_WIDTH - 64) / 2;  // X offset = (128 - 64) / 2 = 32
      // int y = (SCREEN_HEIGHT - 64) / 2; // Y offset = (64 - 64) / 2 = 0

      // Display the downloaded bitmap
      display.drawBitmap(0, 0, bitmap, 64, 64, WHITE);
      display.display();

      Serial.println("Image displayed successfully.");
    } else {
      Serial.println("Failed to read complete image data.");
    }

    delete[] bitmap;
  } else {
    Serial.println("Failed to download image");
  }
  http.end();

}

void loop() {
  // Nothing to do here
}
