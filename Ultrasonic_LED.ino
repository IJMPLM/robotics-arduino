// Ultrasonic pins
const int trigPin = 9;
const int echoPin = 10;

// LED pins (7 LEDs from pin 2 to 8)
const int left = 2;
const int right = 8;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  for (int x = left; x <= right; x++) {
    pinMode(x, OUTPUT);
  }

  Serial.begin(9600);
}

long getDistance() {
  // Send trigger pulse               
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo pulse
  long duration = pulseIn(echoPin, HIGH);
  // Convert to cm (speed of sound ~343 m/s)
  long distance = duration * 0.034 / 2;
  return distance;
}

void updateLEDs(int count) {
  // Turn ON first 'count' LEDs from left to right
  for (int i = 0; i < 7; i++) {
    if (i < count) {
      digitalWrite(left + i, HIGH);
    } else {
      digitalWrite(left + i, LOW);
    }
  }
}

void loop() {
  long distance = getDistance();
  Serial.println(distance);

  // Map distance range (5–35 cm) to LED count (0–7)
  int ledCount = map(distance, 50, 10, 0, 7);

  // Constrain to valid range
  ledCount = constrain(ledCount, 0, 7);

  updateLEDs(ledCount);

  delay(100); // small delay for stability
}
