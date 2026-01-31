/*
  Pattern 1: Running Light (Back and Forth)
*/

const int ledPins[] = {2, 3, 4, 5, 6, 7, 8}; 
const int numLeds = 7;
const int delayTime = 100; // Change this to make it faster/slower

void setup() {
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  // Move Left to Right
  for (int i = 0; i < numLeds; i++) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }

  // Move Right to Left
  // We start from numLeds-2 and end at 1 to prevent double-flashing the edges
  for (int i = numLeds - 2; i > 0; i--) {
    digitalWrite(ledPins[i], HIGH);
    delay(delayTime);
    digitalWrite(ledPins[i], LOW);
  }
}
