#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BALL_RADIUS 5
#define GRAVITY 0.4
#define BOUNCE_FACTOR 0.7

struct Ball {
  float bx;
  float by;
  float velocityY;
};

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Ball balls[10];

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  display.clearDisplay();
  for(int i = 0; i < 10; i++){
    balls[i].bx = random(-100,0);
    balls[i].by = random(-50,0);
    balls[i].velocityY = 0;
  }
}

void loop() {
  // Clear the display
  display.clearDisplay();
  for(int i = 0; i < 10; i++){

      balls[i].bx += 2;
      balls[i].by += balls[i].velocityY;
      balls[i].velocityY += GRAVITY;



  // Check for collision with the ground
  if (balls[i].by + BALL_RADIUS >= SCREEN_HEIGHT) {
    balls[i].by = SCREEN_HEIGHT - BALL_RADIUS;
    balls[i].velocityY = -balls[i].velocityY * BOUNCE_FACTOR;
    
  }
  if(balls[i].bx > SCREEN_WIDTH){
    balls[i].bx = random(-100, -10);
    balls[i].by = random(-100, -10);
     balls[i].velocityY = 0;
  }
  // Draw the ball
  display.fillCircle(balls[i].bx, balls[i].by, BALL_RADIUS, SSD1306_WHITE);
  }
  // Display the updated frame
  display.display();
  delay(5); // Adjust this delay for desired frame rate
  
}
