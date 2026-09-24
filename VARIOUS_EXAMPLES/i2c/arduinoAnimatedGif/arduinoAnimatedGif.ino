#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <I2CRecovery.h>
#include "animation.h"


Adafruit_SSD1306 display(128, 64, &Wire, -1);
int currentFrame = 0;
int incer = 1;


void setup() {
  i2cRecoverBus();
  Wire.begin();
  Wire.setClock(1000000); //1MHz I2C Fast Mode Plus
  
  Wire.setWireTimeout(10000);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // could be 0x3C
  display.clearDisplay(); // Clear the buffer
  Serial.begin(9600);
  
}//end setup


void loop() {
  
  functionArray[currentFrame]();
  currentFrame = currentFrame + incer;
  if(currentFrame == totalFrames-1|| currentFrame < 1){
    incer = incer * -1;
    //currentFrame = 0;
  }
  delay(20);

}//end loop