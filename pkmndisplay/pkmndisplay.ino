// pokemonoftheday.ino
// kyle santos
// adapted from example here on adafruit website https://learn.adafruit.com/monochrome-oled-breakouts/wiring-128x64-oleds
// 9-25-24


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define im_width 128
#define im_height 64
// 'sprite_resized_unedited', 44x48px
// 'NIER_REPLICANTV1P2_PV_NorthernPlains', 128x64px
const unsigned char im_bits [] PROGMEM = {
	0x77, 0xbb, 0xb6, 0xdb, 0x6d, 0xb6, 0xdb, 0x6b, 0x77, 0xed, 0xb6, 0xda, 0xaa, 0xad, 0xfb, 0xeb, 
	0x5d, 0x6d, 0x6d, 0x6d, 0xb6, 0xda, 0xaa, 0xad, 0xae, 0xb5, 0x55, 0x56, 0xdb, 0x7e, 0xad, 0x6d, 
	0x2b, 0xb7, 0xb7, 0xb5, 0x55, 0x55, 0xb5, 0xb5, 0x7b, 0x56, 0xdb, 0x6b, 0x55, 0xab, 0xff, 0xb5, 
	0x16, 0xda, 0xda, 0xd6, 0xb6, 0xad, 0x56, 0xaa, 0xaf, 0xb5, 0x6a, 0xb5, 0x6d, 0x7e, 0xd5, 0xda, 
	0xaa, 0xb7, 0x6d, 0x7b, 0xdb, 0x76, 0xda, 0xdb, 0xb5, 0x5b, 0xad, 0xaa, 0xab, 0xaf, 0xbe, 0xed, 
	0x17, 0x5a, 0xb7, 0xad, 0x6d, 0xab, 0x6d, 0x6d, 0x7e, 0xea, 0xb5, 0x6d, 0xb5, 0xfa, 0xeb, 0x55, 
	0x4a, 0xed, 0xda, 0xb6, 0xb5, 0x6d, 0x56, 0xb7, 0xd5, 0xad, 0x56, 0xb6, 0xde, 0xaf, 0xba, 0xf7, 
	0x25, 0x56, 0xb7, 0x6b, 0xd6, 0xb6, 0xeb, 0xda, 0xba, 0xb5, 0xb5, 0xaa, 0xab, 0xfd, 0xd7, 0x5b, 
	0x53, 0x55, 0x6d, 0xb5, 0x5b, 0xdb, 0x5d, 0x6f, 0xee, 0xd6, 0xd6, 0xdb, 0xfe, 0xb7, 0x7a, 0xad, 
	0x09, 0x6a, 0xda, 0xae, 0xed, 0x55, 0xeb, 0xb5, 0x55, 0x5a, 0xbb, 0x6d, 0x57, 0xfd, 0xab, 0x6b, 
	0x55, 0xb7, 0x6a, 0xd5, 0x55, 0x6e, 0xbd, 0x5b, 0x7b, 0xab, 0x55, 0x57, 0xfa, 0xab, 0x6d, 0xad, 
	0x02, 0xaa, 0xad, 0x5b, 0x6e, 0xb5, 0xab, 0xed, 0xad, 0x75, 0xad, 0xba, 0xaf, 0xfd, 0xb5, 0x57, 
	0x29, 0x5b, 0x6b, 0x6d, 0xb5, 0xdb, 0x7e, 0xdb, 0x76, 0xda, 0xf6, 0xdf, 0xfb, 0x56, 0xd6, 0xba, 
	0x05, 0x55, 0xad, 0xaa, 0xae, 0xad, 0xab, 0xf6, 0xdb, 0x6f, 0x5b, 0x6a, 0xad, 0xfb, 0x6a, 0xd7, 
	0x50, 0xd5, 0x75, 0x5b, 0x72, 0xd6, 0xde, 0xbd, 0xef, 0xb5, 0x6d, 0xbd, 0x7f, 0x56, 0xb5, 0x5a, 
	0x0a, 0x55, 0x16, 0xd6, 0xdd, 0x7b, 0x77, 0xeb, 0x5a, 0xdb, 0xb6, 0xd5, 0x95, 0xfb, 0xd6, 0xab, 
	0x01, 0x2a, 0xd5, 0x6d, 0xeb, 0xad, 0xad, 0x7f, 0xf7, 0x76, 0xdb, 0xfa, 0xdf, 0x55, 0x6a, 0xd5, 
	0x24, 0x25, 0x55, 0x57, 0x5d, 0x56, 0xfb, 0xd5, 0x5d, 0xdd, 0xae, 0xab, 0x53, 0xef, 0x5b, 0x5d, 
	0x00, 0x92, 0xaa, 0xaa, 0xeb, 0xb5, 0xae, 0xff, 0xf7, 0x77, 0xf7, 0x55, 0x55, 0x7a, 0xed, 0x6a, 
	0x10, 0x25, 0x11, 0x5d, 0x9d, 0x6e, 0xdb, 0xaa, 0xbd, 0xdd, 0x5d, 0x55, 0x5a, 0xaf, 0x56, 0xab, 
	0x04, 0x88, 0xaa, 0xab, 0x67, 0xbb, 0x7e, 0xff, 0xd7, 0x77, 0xf6, 0xaa, 0xab, 0x55, 0xbb, 0x5d, 
	0x20, 0x02, 0x29, 0x55, 0x5a, 0xef, 0xaf, 0xaa, 0xfd, 0xdd, 0x5a, 0xd6, 0xd5, 0x6a, 0xd5, 0x65, 
	0x09, 0x29, 0x4a, 0x2a, 0xaa, 0xb5, 0x74, 0xed, 0x17, 0x77, 0xed, 0x55, 0x5a, 0xad, 0x55, 0x5b, 
	0x00, 0x22, 0x55, 0x4a, 0xaa, 0xda, 0xaa, 0x95, 0x6a, 0xda, 0xb5, 0x55, 0x55, 0x56, 0xbb, 0x55, 
	0x29, 0x05, 0x48, 0xa9, 0x55, 0x55, 0x55, 0x55, 0x55, 0x6e, 0xd5, 0x55, 0x55, 0xaa, 0xcd, 0xad, 
	0x00, 0x11, 0x55, 0x12, 0x55, 0x55, 0x54, 0xaa, 0x92, 0x93, 0x55, 0x55, 0x56, 0xb5, 0x55, 0x74, 
	0x4a, 0x84, 0xa9, 0x49, 0x2a, 0xd5, 0x52, 0x44, 0x4a, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0x8a, 
	0x00, 0x10, 0x55, 0x54, 0x92, 0xaa, 0x95, 0x32, 0xaa, 0xaa, 0xaa, 0xad, 0x6a, 0xab, 0x55, 0x62, 
	0x95, 0x04, 0x96, 0x42, 0x4a, 0x54, 0x52, 0x8a, 0x52, 0x94, 0xaa, 0xaa, 0xb5, 0x54, 0xaa, 0x95, 
	0x00, 0xa2, 0x0a, 0xa9, 0x25, 0x22, 0xa8, 0x51, 0x4a, 0x52, 0xa5, 0x55, 0x5a, 0xaa, 0x92, 0x54, 
	0x4a, 0x08, 0x65, 0x24, 0x48, 0x95, 0x0b, 0x2a, 0xa5, 0x55, 0x54, 0xab, 0x6a, 0xd5, 0x55, 0x2a, 
	0x80, 0x81, 0x12, 0x92, 0x25, 0x52, 0xa8, 0x85, 0x12, 0x49, 0x15, 0x55, 0xbd, 0x28, 0x24, 0xa2, 
	0x0a, 0x24, 0x29, 0x55, 0x10, 0x08, 0x55, 0x50, 0xa9, 0x54, 0xa5, 0x2a, 0xd6, 0x92, 0x91, 0x2d, 
	0x40, 0x80, 0x8a, 0x40, 0x8b, 0x55, 0x22, 0x0a, 0xa4, 0x92, 0x92, 0xa5, 0x75, 0x49, 0x04, 0x14, 
	0x12, 0x12, 0x24, 0x95, 0x24, 0x22, 0x94, 0xa2, 0x49, 0x4d, 0x49, 0x29, 0x2c, 0x24, 0x51, 0x42, 
	0x80, 0x40, 0x82, 0x48, 0x48, 0x88, 0x52, 0x54, 0xa4, 0x24, 0xaa, 0x94, 0xaa, 0x82, 0x08, 0x10, 
	0x25, 0x08, 0x51, 0x22, 0x85, 0x55, 0x49, 0x09, 0x12, 0x92, 0x10, 0x4a, 0x94, 0x51, 0x42, 0x44, 
	0x80, 0x22, 0x8a, 0x10, 0x50, 0x49, 0x24, 0xa4, 0xa5, 0x4a, 0xcb, 0x24, 0xa2, 0x0a, 0x28, 0x02, 
	0x00, 0x80, 0x54, 0xa5, 0x25, 0x24, 0x92, 0x11, 0x28, 0xa9, 0x24, 0xda, 0x9b, 0xa4, 0x82, 0xad, 
	0x24, 0x05, 0x4a, 0x89, 0x09, 0x11, 0x40, 0xa4, 0x4a, 0x44, 0x8b, 0x55, 0x48, 0x72, 0x28, 0x50, 
	0x03, 0x41, 0x60, 0x52, 0x44, 0xa4, 0x2a, 0x09, 0x55, 0x29, 0x20, 0xae, 0xf5, 0x09, 0x42, 0x05, 
	0x05, 0x08, 0x3a, 0xa8, 0x12, 0x09, 0x00, 0x85, 0x40, 0x92, 0x4a, 0x21, 0x04, 0xa5, 0x71, 0x54, 
	0x50, 0x01, 0x15, 0x16, 0x89, 0x50, 0xa5, 0x3b, 0x6a, 0x4d, 0x29, 0x48, 0x52, 0x53, 0x4a, 0x20, 
	0x04, 0x40, 0x55, 0x6a, 0x25, 0x25, 0xd1, 0x17, 0xa9, 0x10, 0x40, 0x12, 0x03, 0xbd, 0xa0, 0x82, 
	0xa1, 0x11, 0x16, 0xa9, 0x16, 0xaa, 0x55, 0x59, 0x51, 0x25, 0x15, 0x44, 0xa8, 0x65, 0x54, 0x28, 
	0x08, 0xa4, 0x15, 0x6c, 0x9a, 0xa0, 0x01, 0x25, 0x54, 0x92, 0x48, 0x28, 0x03, 0x55, 0x01, 0x00, 
	0x42, 0x01, 0x0a, 0xb5, 0x2b, 0x55, 0xd0, 0x15, 0x50, 0x00, 0x02, 0xa5, 0x55, 0x48, 0xa8, 0x52, 
	0x10, 0x54, 0xa1, 0x4a, 0x9a, 0xd6, 0x8a, 0x8a, 0xa5, 0x29, 0x20, 0x10, 0x00, 0x12, 0x02, 0x80, 
	0x85, 0x00, 0x08, 0x2a, 0x89, 0x2a, 0xf5, 0x49, 0x20, 0x04, 0x0a, 0x84, 0x4a, 0x45, 0x48, 0xaa, 
	0x40, 0x15, 0x42, 0x90, 0x52, 0x92, 0x16, 0xaa, 0xaa, 0x90, 0x80, 0x21, 0x00, 0x25, 0x02, 0x11, 
	0x2a, 0xa2, 0x29, 0x4a, 0x08, 0x49, 0x49, 0x55, 0x54, 0xaa, 0x6a, 0xda, 0xb5, 0x90, 0xa9, 0x6d, 
	0x04, 0x49, 0x44, 0x01, 0x45, 0x10, 0x24, 0x2a, 0xa5, 0x55, 0x55, 0x55, 0x4a, 0x56, 0x96, 0xa5, 
	0x55, 0x14, 0x22, 0xaa, 0x28, 0xa5, 0x49, 0x4a, 0xaa, 0xaa, 0xaa, 0xaa, 0xb5, 0x52, 0x55, 0x52, 
	0x52, 0x41, 0x14, 0x00, 0x92, 0x09, 0x24, 0xa4, 0x49, 0x4a, 0x55, 0x55, 0x55, 0x5b, 0x6a, 0x95, 
	0x09, 0x48, 0xa1, 0x24, 0x01, 0x54, 0x92, 0x12, 0x94, 0x22, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xa8, 
	0x22, 0x12, 0x08, 0x01, 0x14, 0x02, 0x21, 0x48, 0x22, 0x94, 0x11, 0x2a, 0xaa, 0xd5, 0x55, 0x55, 
	0x48, 0x41, 0x25, 0x48, 0x00, 0x20, 0x88, 0x22, 0x88, 0x41, 0x4a, 0x95, 0x55, 0x52, 0x88, 0x24, 
	0x85, 0x2a, 0x52, 0x01, 0x21, 0x0a, 0x55, 0x48, 0x42, 0x14, 0x20, 0x42, 0x12, 0x9a, 0x6a, 0x0a, 
	0x10, 0x95, 0x48, 0x12, 0x48, 0x40, 0x80, 0x05, 0x29, 0x42, 0x8a, 0x29, 0x49, 0x45, 0x28, 0x81, 
	0x4a, 0x41, 0x2a, 0x84, 0x85, 0x01, 0x25, 0x50, 0x00, 0x28, 0x21, 0x44, 0x24, 0x29, 0x4a, 0xa9, 
	0x20, 0x2a, 0x48, 0x28, 0x50, 0x54, 0x08, 0x24, 0x15, 0x05, 0x4a, 0x12, 0x91, 0x44, 0xa9, 0x25, 
	0x15, 0x40, 0x01, 0x42, 0x8a, 0x02, 0xa5, 0x01, 0x40, 0xa0, 0x24, 0x89, 0x44, 0x22, 0x24, 0xa9, 
	0x40, 0x94, 0x00, 0x10, 0x21, 0x50, 0x40, 0xa4, 0x04, 0x15, 0x92, 0x55, 0x21, 0x11, 0x12, 0x44, 
	0x15, 0x21, 0x55, 0x4a, 0x88, 0x2a, 0x84, 0x01, 0x21, 0x42, 0x49, 0x08, 0x5a, 0x8a, 0xaa, 0xaa
};

#define margin_imgTxt 2
#define margine_txtLines 1 

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }


  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  display.drawBitmap(0, 0, im_bits, im_width, im_height, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

}
