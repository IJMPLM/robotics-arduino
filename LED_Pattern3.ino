/*
  Pattern 3: Running Light with Blinking Counter
*/

int ledPins[] = {2, 3, 4, 5, 6, 7, 8};
int totalLeds = 7;

void setup() {
  for (int i = 0; i < totalLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
}

void loop() {
  for (int groupSize = 1; groupSize <= totalLeds; groupSize++) {
    // Loop for LED Run
    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(ledPins[i], HIGH);
      delay(100); 
      digitalWrite(ledPins[i], LOW);
    }

    delay(100); // Delay before blinking

    for (int blinkCount = 0; blinkCount < groupSize; blinkCount++) {
      // Loop for blink counter
      for (int i = 0; i < groupSize; i++) {
        digitalWrite(ledPins[i], HIGH);
      }
      delay(200);

     // Loop to turn off
      for (int i = 0; i < groupSize; i++) {
        digitalWrite(ledPins[i], LOW);
      }
      delay(200);
    }
    
    delay(400); // Pause before next run
  }

  // After 7 LEDs blink 7 times, the loop restarts to 1st led
}