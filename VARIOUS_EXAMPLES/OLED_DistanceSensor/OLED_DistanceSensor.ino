#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VL53L0X.h>
#include "bitmaps.h";

VL53L0X distanceSensor;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)){
    Serial.println("no connection to display!");
    while (1) {}
  }

  display.clearDisplay();
  distanceSensor.setTimeout(500);
  if (!distanceSensor.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  
}

void loop() {
  int dist = distanceSensor.readRangeSingleMillimeters();
  if(dist > 1000){
    display.clearDisplay();
    display.drawBitmap(0, 0,  eye2, 128, 64, WHITE);
    display.display();
  }else{
      display.clearDisplay();
      display.drawBitmap(0, 0,  eye1, 128, 64, WHITE);
      display.display();
  }
  delay(100);
}
