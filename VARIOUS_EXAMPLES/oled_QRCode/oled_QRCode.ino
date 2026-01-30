//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "qrcode.h"


#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);
QRCode qrcode;


void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay(); 
}
void loop() {
 
 
while (Serial.available() == 0) {}     //wait for data available
  String teststr = Serial.readString();  //read until timeout
  teststr.trim();                        // remove any \r \n whitespace at the end of the String
  int str_len = teststr.length() + 1; 

  // Prepare the character array (the buffer) 
  char char_array[str_len];
  teststr.toCharArray(char_array, str_len);
  drawQRCode(char_array);
  delay(1000);
}

void drawQRCode(char* s){
   uint8_t PROGMEM qrcodeData[qrcode_getBufferSize(8)];
    qrcode_initText(&qrcode, qrcodeData, 8, ECC_HIGH, s);
      delay(100);
    display.clearDisplay();
      for (uint8_t yy = 0; yy < qrcode.size; yy++) {
        // Each horizontal module
        for (uint8_t xx = 0; xx < qrcode.size; xx++) {

            // Print each module (UTF-8 \u2588 is a solid block)
            if(qrcode_getModule(&qrcode, xx, yy) == true){
              display.fillRect(xx, yy, 1, 1, SSD1306_WHITE);
            }
            
           // Serial.print(qrcode_getModule(&qrcode, x, y) ? "\u2588\u2588": "  ");

        }
    }
      display.display();
}
