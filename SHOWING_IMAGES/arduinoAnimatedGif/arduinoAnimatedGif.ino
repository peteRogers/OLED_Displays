#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lines.h"
#include "displayManager.h"


Adafruit_SSD1306 display(128, 64, &Wire, -1);
int currentFrame = 0;
int incer = 1;


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // could be 0x3C
  display.clearDisplay(); // Clear the buffer
  //Serial.begin(9600);
  
}//end setup


void loop() {
  
  functionArray[currentFrame]();
  currentFrame = currentFrame + incer;
  if(currentFrame == totalFrames-1|| currentFrame < 1){
    incer = incer * -1;
  }


}//end loop