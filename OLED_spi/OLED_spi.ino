// Basic starter sketch for a 128x64 SPI OLED (SSD1306), using the board's
// real hardware SPI peripheral rather than bit-banged software SPI (which
// this file used to use - much slower, and pins arbitrary in a way that
// doesn't reflect how the hardware actually works). See
// VARIOUS_EXAMPLES/OLED_SineWave_SPI for more on why hardware SPI is
// faster and avoids screen tearing at high frame rates.
//
// MOSI/SCK are fixed hardware SPI pins on the board (D11/D13 on an
// UNO R4). DC, RST and CS can be any free digital pins - just match
// these #defines to how you wired it.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_DC  9   // data/command select
#define OLED_RST 8   // reset
#define OLED_CS  10  // chip select

// &SPI = use the board's hardware SPI peripheral, clocked at 8MHz.
Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // don't continue, the display didn't initialise
  }

  display.clearDisplay();
}//end setup function


void loop() {
  display.clearDisplay();
  display.fillCircle(64, 32, 10, WHITE);
  display.display();
}//end loop function



////EXTRA FUNCTIONS

void drawEllipse(int16_t cx, int16_t cy, int16_t rx, int16_t ry, uint16_t color) {
  int16_t px = cx + rx, py = cy;
  for (int a = 1; a <= 36; a++) {
    float t = a * PI / 18;
    int16_t x = cx + rx * cos(t), y = cy + ry * sin(t);
    display.drawLine(px, py, x, y, color);
    px = x; py = y;
  }
}

void drawArc(int16_t cx, int16_t cy, int16_t rx, int16_t ry, int startDeg, int endDeg, uint16_t color) {
  int16_t px = cx + rx * cos(startDeg * DEG_TO_RAD), py = cy + ry * sin(startDeg * DEG_TO_RAD);
  for (int a = startDeg + 5; a <= endDeg; a += 5) {
    int16_t x = cx + rx * cos(a * DEG_TO_RAD), y = cy + ry * sin(a * DEG_TO_RAD);
    display.drawLine(px, py, x, y, color);
    px = x; py = y;
  }
}
