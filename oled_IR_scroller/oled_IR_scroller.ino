// OLED_spi with a scrolling blocky landscape along the bottom, and a ball
// on the left that moves up and down with an IR sensor on A0.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 128x64 screen on hardware SPI at 8MHz. DC = pin 9, RST = pin 8, CS = pin 10.
Adafruit_SSD1306 display(128, 64, &SPI, 9, 8, 10, 8000000UL);

// The landscape: one block per entry, repeated forever.
int blockW[] = {42, 26, 50, 34, 24, 40, 30, 46};
int blockH[] = {12, 26,  8, 34, 16, 22, 10, 30};

int firstBlock = 0;  // which block is at the left edge of the screen
int offset = 0;      // how many pixels of that block have scrolled off


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC);
}


void loop() {
  display.clearDisplay();

  // Draw blocks from the left edge until the screen is full.
  int x = -offset;
  int i = firstBlock;
  while (x < 128) {
    display.fillRect(x, 64 - blockH[i], blockW[i], blockH[i], WHITE);
    x += blockW[i];
    i = (i + 1) % 8;
  }

  // Scroll left one pixel. Once a block has fully gone, move on to the next.
  offset++;
  if (offset == blockW[firstBlock]) {
    offset = 0;
    firstBlock = (firstBlock + 1) % 8;
  }

  // Ball: sensor reading 50-900 maps to bottom-top of the screen.
  int ballY = map(analogRead(A0), 50, 900, 58, 5);
  ballY = constrain(ballY, 5, 58);
  display.fillCircle(20, ballY, 5, WHITE);

  display.display();
  delay(20);
}
