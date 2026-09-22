// Same animated GIF playback as arduinoAnimatedGif.ino, but driven over true
// hardware SPI instead of I2C - see VARIOUS_EXAMPLES/OLED_SineWave_SPI for
// why that's faster and avoids tearing. animation.h itself doesn't care which
// bus is used (it just draws to an extern "display" object), so it's copied
// here unchanged.
//
// MOSI/SCK are fixed hardware SPI pins on the board. DC, RST and CS can be
// any free digital pins - just match these #defines to how you wired it.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "animation.h"

#define OLED_DC  9   // data/command select
#define OLED_RST 8   // reset
#define OLED_CS  10  // chip select

// &SPI = use the board's hardware SPI peripheral, clocked at 8MHz.
Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

int currentFrame = 0;
int incer = 1;

void setup() {
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // don't continue, the display didn't initialise
  }
  display.clearDisplay();
}//end setup

void loop() {

  functionArray[currentFrame]();
  currentFrame = currentFrame + incer;
  if (currentFrame == totalFrames - 1 || currentFrame < 1) {
    incer = incer * -1;
  }
  delay(20);

}//end loop
