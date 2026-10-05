
//importing Libraries
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//wiring for display
#define OLED_DC  9   
#define OLED_RST 8   
#define OLED_CS  10  




Adafruit_SSD1306 display(128, 64, &SPI, OLED_DC, OLED_RST, OLED_CS, 8000000UL);

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  
}//end setup function


void loop() {
  display.clearDisplay();
  display.drawCircle(64, 32, 20, WHITE);
  display.display();
}//end loop function



////EXTRA FUNCTIONS

void drawEllipse(int16_t cx, int16_t cy, int16_t rx, int16_t ry, uint16_t color) {
  int16_t px = cx + rx, py = cy;
  for (int a = 1; a <= 36; a++) {
    float t = a * PI / 18;
    int16_t x = cx + rx * cos(t), y = cy + ry * sin(t);
    display.drawLine(px, py, x, y, color);
    px = x; py = y;
  }
}

void drawArc(int16_t cx, int16_t cy, int16_t rx, int16_t ry, int startDeg, int endDeg, uint16_t color) {
  int16_t px = cx + rx * cos(startDeg * DEG_TO_RAD), py = cy + ry * sin(startDeg * DEG_TO_RAD);
  for (int a = startDeg + 5; a <= endDeg; a += 5) {
    int16_t x = cx + rx * cos(a * DEG_TO_RAD), y = cy + ry * sin(a * DEG_TO_RAD);
    display.drawLine(px, py, x, y, color);
    px = x; py = y;
  }
}
