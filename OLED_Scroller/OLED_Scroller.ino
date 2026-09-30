
//importing Libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//wiring for display
#define OLED_DC  9   
#define OLED_RST 8   
#define OLED_CS  10  

//global variables
int textSize = 3;  // change this to resize the text - scrolling adjusts to match
char message[] = "start lalla la l al al END";
int scrollDistance = strlen(message) * 6 * textSize - 128;  // text width minus screen width


Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(textSize);
  display.setTextWrap(false);
}//end setup function


void loop() {
  display.clearDisplay();
  int reading = constrain(analogRead(A0), 50, 660);
  int x = map(reading, 50, 660, -scrollDistance, 0);
  display.setCursor(x, 20);
  display.print(message);
  display.display();
}//end loop function
