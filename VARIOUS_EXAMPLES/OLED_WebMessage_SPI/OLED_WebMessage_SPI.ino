// Mirrors text typed live in a web page onto an SSD1306 OLED driven by true
// hardware SPI. The web page (web/ folder) constrains typing to the same
// 21-column x 8-row grid the physical screen has and sends the whole grid
// on every keystroke, with '\r' between rows - so this sketch just needs to
// split on '\r' and print each row at its matching y position. No word-wrap
// needed here: the browser already enforces the same line breaks you'll see
// on the board.
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

#define BAUD_RATE 115200 // must match the baudRate used in web/serial.js

#define CHAR_WIDTH  6                        // default font glyph + spacing, in pixels
#define CHAR_HEIGHT 8
#define ROWS (SCREEN_HEIGHT / CHAR_HEIGHT)    // must match ROWS in web/sketch.js

// &SPI = use the board's hardware SPI peripheral, clocked at 8MHz.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

void setup() {
  Serial.begin(BAUD_RATE);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    for (;;)
      ; // don't continue, the display didn't initialise
  }

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setTextWrap(false); // rows already come pre-wrapped from the browser

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Waiting for a message...");
  display.display();
}

void loop() {
  if (Serial.available()) {
    String grid = Serial.readStringUntil('\n');

    display.clearDisplay();

    int row = 0;
    int rowStart = 0;
    for (int i = 0; i <= (int)grid.length() && row < ROWS; i++) {
      if (i == (int)grid.length() || grid[i] == '\r') {
        display.setCursor(0, row * CHAR_HEIGHT);
        display.print(grid.substring(rowStart, i));
        row++;
        rowStart = i + 1;
      }
    }

    display.display();
    Serial.println("OK"); // let the web page know this update was received
  }
}
