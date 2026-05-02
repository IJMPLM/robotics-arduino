#include <Arduino.h>

// --- COLOR SENSOR PINS ---
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define sensorOut 10

// --- IR PROXIMITY SENSOR PIN ---
#define irProxPin A0   // analog input from IR sensor (e.g., Sharp GP2Y0A21)

// Calibration ranges (adjust after testing)
int minRed = 50;   int maxRed = 600;
int minGreen = 50; int maxGreen = 600;
int minBlue = 50;  int maxBlue = 600;

// Tolerance for ratio comparisons
int tolerance = 5; // percentage points

// Proximity threshold (adjust based on your sensor and distance)
int proxThreshold = 650; // lower = closer, tune experimentally

void setup() {
  Serial.begin(9600);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  Serial.println("Color Sensor + IR Proximity Mode");
}

void loop() {
  // --- Check proximity first ---
  int proxValue = analogRead(irProxPin);
  Serial.print("Proximity value: ");
  Serial.println(proxValue);

  if (proxValue > proxThreshold) {
    // No object detected
    Serial.println("Detected: NONE (no object)");
    delay(500);
    return; // skip color reading
  }

  // --- Read Red ---
  digitalWrite(S2, LOW); digitalWrite(S3, LOW);
  int rRaw = pulseIn(sensorOut, LOW);
  int rVal = constrain(map(rRaw, minRed, maxRed, 255, 0), 0, 255);

  // --- Read Green ---
  digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
  int gRaw = pulseIn(sensorOut, LOW);
  int gVal = constrain(map(gRaw, minGreen, maxGreen, 255, 0), 0, 255);

  // --- Read Blue ---
  digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
  int bRaw = pulseIn(sensorOut, LOW);
  int bVal = constrain(map(bRaw, minBlue, maxBlue, 255, 0), 0, 255);

  // --- Normalize to ratios ---
  int sum = rVal + gVal + bVal;
  float rRatio = (sum > 0) ? (100.0 * rVal / sum) : 0;
  float gRatio = (sum > 0) ? (100.0 * gVal / sum) : 0;
  float bRatio = (sum > 0) ? (100.0 * bVal / sum) : 0;

  // --- Determine dominant color with tolerance ---
  char detected = 'N';
  if (gRatio >= rRatio - tolerance && gRatio >= bRatio - tolerance && gRatio >= rRatio && gRatio >= bRatio) {
    detected = 'G';
  } else if (rRatio >= gRatio - tolerance && rRatio >= bRatio - tolerance && rRatio >= gRatio && rRatio >= bRatio) {
    detected = 'R';
  } else if (bRatio >= rRatio - tolerance && bRatio >= gRatio - tolerance && bRatio >= rRatio && bRatio >= gRatio) {
    detected = 'B';
  }

  // --- Print results ---
  Serial.print("R: "); Serial.print(rVal);
  Serial.print(" G: "); Serial.print(gVal);
  Serial.print(" B: "); Serial.print(bVal);
  Serial.print(" | Ratios -> R: "); Serial.print(rRatio, 1);
  Serial.print("% G: "); Serial.print(gRatio, 1);
  Serial.print("% B: "); Serial.print(bRatio, 1);
  Serial.print(" | Detected: ");
  if (detected == 'R') Serial.println("RED");
  else if (detected == 'G') Serial.println("GREEN");
  else if (detected == 'B') Serial.println("BLUE");
  else Serial.println("NONE");

  delay(500);
}
