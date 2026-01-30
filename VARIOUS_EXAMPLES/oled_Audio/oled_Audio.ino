
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//audio samples
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
//screen setup
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
}


void loop(){
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;

   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    peakToPeak = map(peakToPeak,0, 400, 0, 128);
    display.clearDisplay();
    display.fillRect(0,0, peakToPeak, 64, SSD1306_WHITE);
    display.display();
    //delay(2);
}



