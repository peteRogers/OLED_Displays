// Basic starter sketch for a 128x64 I2C OLED (SSD1306).
// Draws a text banner with a ball bouncing back and forth underneath it.

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <I2CRecovery.h>   // needs libraries/I2CRecovery installed in your Arduino libraries folder

// width, height, I2C bus, reset pin (-1 = no dedicated reset pin)
// Wire1, not Wire: on the UNO R4 WiFi the Qwiic/STEMMA QT connector is a
// separate I2C bus from the header pins.
Adafruit_SSD1306 display(128, 64, &Wire1, -1);

int ballX = 0;       // ball's current horizontal position
int ballSpeed = 2;   // pixels moved per frame; sign controls direction

void setup() {
  i2cRecoverBus(Wire1);   // un-jam the Qwiic bus if a previous run left it mid-transaction
  Wire1.begin();
  Wire1.setClock(1000000);   // 1MHz I2C Fast Mode Plus
  Wire1.setWireTimeout(10000);
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); // try 0x3C here if the screen stays blank
  display.clearDisplay();
}//end setup


void loop() {
  display.clearDisplay();

  // Text banner along the top
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Hello, OLED!");

  // Ball bouncing left/right underneath the text
  display.fillCircle(ballX, 40, 8, WHITE);
  ballX = ballX + ballSpeed;
  if (ballX <= 0 || ballX >= 128) {
    ballSpeed = -ballSpeed;   // hit an edge, so reverse direction
  }

  display.display();   // send everything drawn above to the screen
  delay(20);
}//end loop
