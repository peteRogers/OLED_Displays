// Same animated sine wave as OLED_SineWave.ino, but driven over true hardware
// SPI instead of I2C. Hardware SPI uses the board's dedicated SPI peripheral
// (not bit-banged pins like OLED_spi.ino), so a full frame - 1024 bytes for a
// 128x64 display - takes roughly 1ms at 8MHz, versus several ms over I2C.
// That much shorter transfer window is what actually fixes the tearing seen
// on I2C at high frame rates, rather than any one setting in the sketch.
//
// MOSI/SCK are fixed hardware SPI pins on the board. DC, RST and CS can be
// any free digital pins - just match these #defines to how you wired it.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

#define OLED_DC  9   // data/command select
#define OLED_RST 8   // reset
#define OLED_CS  10  // chip select

// &SPI = use the board's hardware SPI peripheral, clocked at 8MHz.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

float phase = 0;        // shifts the wave sideways each frame, creating motion
float amplitude = 20;   // how tall the wave is, in pixels
float frequency = 0.15; // how many wave cycles fit across the screen

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // don't continue, the display didn't initialise
  }
  display.clearDisplay();
}

void loop() {
  display.clearDisplay();

  int previousY = SCREEN_HEIGHT / 2;
  for (int x = 0; x < SCREEN_WIDTH; x++) {
    int y = (SCREEN_HEIGHT / 2) + amplitude * sin(frequency * x + phase);
    if (x > 0) {
      display.drawLine(x - 1, previousY, x, y, WHITE);
    }
    previousY = y;
  }

  display.display();
  phase += 0.4;   // same fast pacing as the I2C version, to compare directly
  delay(10);
}
