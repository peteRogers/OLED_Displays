// Draws an animated sine wave scrolling across a 128x64 I2C OLED (SSD1306).
// Uses the primary I2C bus (Wire) - see OLED_STARTER for the Wire1/Qwiic version.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <I2CRecovery.h>  

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

// clkDuring/clkAfter (5th/6th args) keep the I2C bus at the 1MHz set below during
// each display() write; without them the library resets it to 400kHz every frame.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1, 1000000, 400000);

float phase = 0;        // shifts the wave sideways each frame, creating motion
float amplitude = 20;   // how tall the wave is, in pixels
float frequency = 0.15; // how many wave cycles fit across the screen

void setup() {
   i2cRecoverBus(Wire);   // un-jam the Qwiic bus if a previous run left it mid-transaction
  Wire.begin();
  Wire.setClock(1000000);   // 1MHz I2C Fast Mode Plus
  Wire.setWireTimeout(10000);
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // try 0x3C here if the screen stays blank
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
  phase += 0.4;   // advance the wave each frame so it scrolls
  delay(10);      // cap the frame rate so each display() finishes well before the next one starts,
                   // which avoids visible tearing (a full-frame I2C write takes a few ms, and the
                   // panel keeps refreshing from its own RAM while that write is still in flight)
}
