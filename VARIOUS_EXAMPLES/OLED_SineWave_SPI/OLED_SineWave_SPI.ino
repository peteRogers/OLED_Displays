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

float phase = 0; // shifts the wave sideways each frame, creating motion

// amplitude and frequency both breathe between these limits, in sync with speed
const float minSpeed = 0.05,     maxSpeed = 0.6;   // phase step per frame
const float minAmplitude = 8,    maxAmplitude = 28; // wave height, in pixels
const float minFrequency = 0.08, maxFrequency = 0.3; // wave cycles per pixel
const float cycleSeconds = 5.0;  // one full slow->fast->slow lap

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
  // Where we are in one slow->fast->slow lap (0..1), from wall-clock time so
  // it stays smooth regardless of how long each frame takes to draw.
  unsigned long msIntoCycle = millis() % (unsigned long)(cycleSeconds * 1000);
  float t = msIntoCycle / 1000.0; // 0..cycleSeconds
  float speedFactor = (sin(2 * PI * t / cycleSeconds - HALF_PI) + 1) / 2; // 0..1

  float speed = minSpeed + speedFactor * (maxSpeed - minSpeed);
  float amplitude = minAmplitude + speedFactor * (maxAmplitude - minAmplitude);
  float frequency = minFrequency + speedFactor * (maxFrequency - minFrequency);

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

  phase += speed;
  delay(10);
}
