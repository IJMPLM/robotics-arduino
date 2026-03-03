#include <Servo.h>

// Servo pins
const int servoPinX = 9;   // base servo (horizontal)
const int servoPinY = 10;  // bracket servo (vertical)

// Photoresistor pins (analog inputs)
// Mapping based on your reference:
// bottom = 0, right = 1, top = 2, left = 3
const int sensorBottom = A0;
const int sensorRight  = A1;
const int sensorTop    = A2;
const int sensorLeft   = A3;

Servo servoX;
Servo servoY;

// Servo positions
int posX = 90;  // start centered
int posY = 90;

void setup() {
  servoX.attach(servoPinX);
  servoY.attach(servoPinY);

  servoX.write(posX);
  servoY.write(posY);

  Serial.begin(9600);
}

void loop() {
  // Read photoresistors
  int bottom = analogRead(sensorBottom);
  int right  = analogRead(sensorRight);
  int top    = analogRead(sensorTop);
  int left   = analogRead(sensorLeft);

  // Log values for debugging
  Serial.print("Bottom: "); Serial.print(bottom);
  Serial.print(" Right: "); Serial.print(right);
  Serial.print(" Top: "); Serial.print(top);
  Serial.print(" Left: "); Serial.println(left);

  // Compute averages
  int verticalAvgTop    = top;
  int verticalAvgBottom = bottom;
  int horizontalAvgLeft = left;
  int horizontalAvgRight= right;

  // Vertical adjustment (Y servo: 0 = down, 180 = up)
  if (verticalAvgTop > verticalAvgBottom + 50) {
    posY = max(posY - 1.5, 0);     // tilt up
  } else if (verticalAvgBottom > verticalAvgTop + 50) {
    posY = min(posY + 1.5, 180);       // tilt down
  }

  // Horizontal adjustment (X servo: 0 = left, 180 = right)
  if (horizontalAvgLeft > horizontalAvgRight + 50) {
    posX = max(posX - 3, 0);       // move left
  } else if (horizontalAvgRight > horizontalAvgLeft + 50) {
    posX = min(posX + 3, 180);     // move right
  }

  // Update servos
  servoX.write(posX);
  servoY.write(posY);
}
