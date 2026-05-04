#include <Arduino.h>
#include <Servo.h>

// ==========================================
// --- SENSOR PINS & SETTINGS ---
// ==========================================
#define S0 3
#define S1 4
#define S2 5
#define S3 6
#define sensorOut 7

#define irProxPin A0   // analog input from IR sensor (e.g., Sharp GP2Y0A21)

// Calibration ranges (adjust after testing)
int minRed = 50;   int maxRed = 600;
int minGreen = 50; int maxGreen = 600;
int minBlue = 50;  int maxBlue = 600;

// Tolerance for ratio comparisons
int tolerance = 5; // percentage points

// Proximity threshold (adjust based on your sensor and distance)
int proxThreshold = 650; // lower = closer, tune experimentally


// ==========================================
// --- SERVO PINS & SETTINGS ---
// ==========================================
#define BASE_PIN 10
#define JOINT1_PIN 9
#define CLAMP_PIN 8

Servo baseServo;
Servo joint1Servo;
Servo clampServo;

// Current Positions (Start Defaults)
int basePos = 10;
int j1Pos = 40;
int clampPos = 135;

// Target Angles for Bins/Targets
const int targetBlue = 45;
const int targetGreen = 75;
const int targetRed = 120;

// Delay in ms between each degree of movement (Speed Control)
const int moveDelay = 15; 


// ==========================================
// --- SETUP ---
// ==========================================
void setup() {
  Serial.begin(9600);

  // --- Initialize Color Sensor ---
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // --- Initialize Servos ---
  baseServo.attach(BASE_PIN);
  joint1Servo.attach(JOINT1_PIN);
  clampServo.attach(CLAMP_PIN);

  // Move to default positions immediately on startup
  baseServo.write(basePos);
  joint1Servo.write(j1Pos);
  clampServo.write(clampPos);

  Serial.println("--- System Ready: Awaiting Object ---");
}


// ==========================================
// --- MAIN LOOP ---
// ==========================================
void loop() {
  // 1. Check proximity first
  int proxValue = analogRead(irProxPin);

  if (proxValue > proxThreshold) {
    // No object detected, exit loop and check again
    delay(200); 
    return; 
  }

  // 2. Object Detected! Allow a brief moment for it to settle
  Serial.println("\nObject Detected! Scanning color...");
  delay(500); 

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

  // --- Determine dominant color ---
  char detected = 'N';
  if (gRatio >= rRatio - tolerance && gRatio >= bRatio - tolerance && gRatio >= rRatio && gRatio >= bRatio) {
    detected = 'G';
  } else if (rRatio >= gRatio - tolerance && rRatio >= bRatio - tolerance && rRatio >= gRatio && rRatio >= bRatio) {
    detected = 'R';
  } else if (bRatio >= rRatio - tolerance && bRatio >= gRatio - tolerance && bRatio >= rRatio && bRatio >= gRatio) {
    detected = 'B';
  }

  // --- Execute Action Based on Color ---
  if (detected == 'R') {
    Serial.println("Result: RED. Executing Red Sequence.");
    runSequence(targetRed);
  } 
  else if (detected == 'G') {
    Serial.println("Result: GREEN. Executing Green Sequence.");
    runSequence(targetGreen);
  } 
  else if (detected == 'B') {
    Serial.println("Result: BLUE. Executing Blue Sequence.");
    runSequence(targetBlue);
  } 
  else {
    Serial.println("Result: NONE/UNCLEAR. Ignoring object.");
    delay(1000); // Wait before scanning again to avoid spam
  }
}


// ==========================================
// --- SERVO FUNCTIONS ---
// ==========================================

// Steps the servo 1 degree at a time to prevent violent jerking
void moveServoGradual(Servo &servo, int &currentPosition, int targetPosition) {
  if (currentPosition < targetPosition) {
    // Move up to target
    for (int pos = currentPosition; pos <= targetPosition; pos++) {
      servo.write(pos);
      delay(moveDelay);
    }
  } else {
    // Move down to target
    for (int pos = currentPosition; pos >= targetPosition; pos--) {
      servo.write(pos);
      delay(moveDelay);
    }
  }
  // Update the global tracking variable
  currentPosition = targetPosition; 
}

// Executes the pick and place sequence
void runSequence(int targetBaseAngle) {
  delay(200); 

  Serial.println("   -> 1. Lowering claw...");
  moveServoGradual(joint1Servo, j1Pos, 70);
  delay(300); // Brief pause to let mechanics settle

  Serial.println("   -> 2. Closing clamp...");
  moveServoGradual(clampServo, clampPos, 0);
  delay(500); // Give it time to securely grip the object

  Serial.println("   -> 3. Raising claw back up...");
  moveServoGradual(joint1Servo, j1Pos, 40);
  delay(300);

  Serial.print("   -> 4. Moving base to target angle: ");
  Serial.println(targetBaseAngle);
  moveServoGradual(baseServo, basePos, targetBaseAngle);
  delay(300);

  Serial.println("   -> 5. Opening clamp to drop object...");
  moveServoGradual(clampServo, clampPos, 135);
  delay(500);

  Serial.println("   -> 6. Resetting to start defaults...");
  // First move the base back to 10
  moveServoGradual(baseServo, basePos, 10);
  // Ensure clamp and joint are at default
  moveServoGradual(joint1Servo, j1Pos, 40);
  moveServoGradual(clampServo, clampPos, 135);
  
  Serial.println("Sequence Complete! Resuming scanning...\n");
  delay(500); // Short buffer before scanning for the next object
}