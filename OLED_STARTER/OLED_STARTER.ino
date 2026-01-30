//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);


void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); //could be 0x3C
  display.clearDisplay();
}//end setup


void loop() {
  display.clearDisplay();
  //drawing code here
  display.fillRect(0, 0, 128, 64, WHITE);
  display.display();//drawing code sent to display
  delay(10);
}//end loop
