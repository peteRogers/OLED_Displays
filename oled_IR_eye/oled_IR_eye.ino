// A simple eye (outline + pupil) that opens and closes with an IR sensor on A0.
// The outline is an ellipse whose height follows the sensor reading.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// 128x64 screen on hardware SPI at 8MHz. DC = pin 9, RST = pin 8, CS = pin 10.
Adafruit_SSD1306 display(128, 64, &SPI, 9, 8, 10, 8000000UL);


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC);
}


void loop() {
  display.clearDisplay();

  // How open the eye is: 0 = shut, 28 = fully open.
  int open = map(analogRead(A0), 50, 900, 0, 28);
  open = constrain(open, 0, 28);

  // Manga-style iris: tall oval iris, tall oval pupil, and two shine spots.
  display.fillEllipse(64, 32, 16, 22, WHITE);  // iris
  display.fillEllipse(64, 32, 7, 11, BLACK);   // pupil
  display.fillCircle(58, 24, 5, BLACK);        // big shine: black ring...
  display.fillCircle(58, 24, 1, WHITE);        // ...with a white dot inside
  display.fillCircle(68, 38, 1, WHITE);        // small shine in the pupil

  // Black out any of the iris that sits above or below the open eye.
  display.fillRect(0, 0, 128, 32 - open, BLACK);
  display.fillRect(0, 33 + open, 128, 31 - open, BLACK);

  // Eye outline: 50 pixels either side of centre, 'open' pixels up and down.
  // if (open == 0) {
  //   display.drawLine(14, 32, 114, 32, WHITE);  // fully shut: just a line
  // } else {
  //   display.drawEllipse(64, 32, 50, open, WHITE);
  // }
  display.drawEllipse(64, 32, 50, open, WHITE);
  display.display();
  delay(20);
}
