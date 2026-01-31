const int left = 2;
const int right = 8;

void setup() {
  for(int x = left; x <= right; x++){
    pinMode(x, OUTPUT);
  }
}

void updateLED(int activePins[], int length){
  // Turn all OFF
  for(int x = left; x <= right; x++){
    digitalWrite(x, LOW);
  }
  // Turn ON active pins
  for(int i = 0; i < length; i++){
    digitalWrite(activePins[i], HIGH);
  }
}

void runAnimation(bool entrance, int activePins[], int length, int nextPin){
  if(entrance){
    // Right → Left sweep
    for(int p = right; p >= left; p--){
      digitalWrite(p, HIGH);
      delay(100);

      // Keep ON if part of stack OR the nextPin
      bool keepOn = (p == nextPin);
      for(int i = 0; i < length; i++){
        if(activePins[i] == p) { keepOn = true; break; }
      }
      if(!keepOn) digitalWrite(p, LOW);
    }
  } else {
    // Left → Right sweep
    for(int p = left; p <= right; p++){
      digitalWrite(p, HIGH);
      delay(100);

      // Keep ON if part of stack OR the nextPin
      bool keepOn = (p == nextPin);
      for(int i = 0; i < length; i++){
        if(activePins[i] == p) { keepOn = true; break; }
      }
      if(!keepOn) digitalWrite(p, LOW);
    }
  }
}

void loop() {
  int stack[7];   // max 7 LEDs (pins 2–8)
  int top = 0;    // stack pointer

  delay(1000);
  // Entrance (push LEDs one by one)
  for(int p = right; p >= left; p--){
    runAnimation(false, stack, top, p);   // sweep before push, keep new LED ON
    stack[top++] = p;                     // push LED
    updateLED(stack, top);                // update display
    delay(100);
  }

  delay(500);

  // Exit (pop LEDs one by one)
  while(top > 0){
    int removed = stack[top-1];           // top LED to remove
    top--;                                // pop LED
    updateLED(stack, top);                // update display
    runAnimation(true, stack, top, -1);   // sweep after removal (no nextPin)
  }
}
