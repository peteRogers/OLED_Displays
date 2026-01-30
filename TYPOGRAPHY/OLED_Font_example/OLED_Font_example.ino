#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/Picopixel.h>//tiny font
#include <Fonts/FreeSansBold9pt7b.h>//bold font
#include <Fonts/FreeSansBoldOblique9pt7b.h>//italic bold font
#include <Fonts/FreeSans12pt7b.h>//Large sans font

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); //0x3C
  display.setFont(&Picopixel);
  display.setTextColor(WHITE);
}

void loop() {
  display.clearDisplay();

  display.setCursor(5, 5);//set text position
  display.setFont(&Picopixel);//choose font
  display.println("tiny text");//write text

  display.setFont(&FreeSansBoldOblique9pt7b);
  display.setCursor(5, 25);
  display.println("bold italic");

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(5, 40);
  display.println("bold");

  display.setFont(&FreeSans12pt7b);
  display.setCursor(5, 55);
  display.println("sans serif");

  display.display();//sends everything to the display
  delay(200);
}

