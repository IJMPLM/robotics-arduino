#include <Servo.h>

// Servo pins
const int servoPinX = 9;   // base servo (horizontal)
const int servoPinY = 10;  // bracket servo (vertical)

// Photoresistor pins (analog inputs)
const int sensorTopLeft     = A0;
const int sensorTopRight    = A1;
const int sensorBottomLeft  = A2;
const int sensorBottomRight = A3;

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
  int tl = analogRead(sensorTopLeft);
  int tr = analogRead(sensorTopRight);
  int bl = analogRead(sensorBottomLeft);
  int br = analogRead(sensorBottomRight);

  // Log values for debugging
  Serial.print("TL: "); Serial.print(tl);
  Serial.print(" TR: "); Serial.print(tr);
  Serial.print(" BL: "); Serial.print(bl);
  Serial.print(" BR: "); Serial.println(br);

  // Compute averages
  int topAvg = (tl + tr) / 2;
  int bottomAvg = (bl + br) / 2;
  int leftAvg = (tl + bl) / 2;
  int rightAvg = (tr + br) / 2;

  // Vertical adjustment
  if (topAvg > bottomAvg + 50) {   // threshold to avoid jitter
    posY = min(posY + 1, 180);     // tilt up
  } else if (bottomAvg > topAvg + 50) {
    posY = max(posY - 1, 0);       // tilt down
  }

  // Horizontal adjustment
  if (leftAvg > rightAvg + 50) {
    posX = min(posX + 1, 180);     // rotate left
  } else if (rightAvg > leftAvg + 50) {
    posX = max(posX - 1, 0);       // rotate right
  }

  // Update servos
  servoX.write(posX);
  servoY.write(posY);

  delay(50); // small delay for stability
}
