#include <Servo.h>

// --- SERVO PINS ---
#define BASE_PIN 13
#define JOINT1_PIN 12
#define JOINT2_PIN 11
#define CLAMP_PIN 10

// --- SERVO OBJECTS ---
Servo baseServo;
Servo joint1Servo;
Servo joint2Servo;
Servo clampServo;

// --- CURRENT POSITIONS ---
int basePos = 90;
int j1Pos = 90;
int j2Pos = 90;
int clampPos = 0;

void setup() {
  Serial.begin(9600);

  // Attach servos to pins
  baseServo.attach(BASE_PIN);
  joint1Servo.attach(JOINT1_PIN);
  joint2Servo.attach(JOINT2_PIN);
  clampServo.attach(CLAMP_PIN);

  // Set to user-defined defaults
  baseServo.write(basePos);
  joint1Servo.write(j1Pos);
  joint2Servo.write(j2Pos);
  clampServo.write(clampPos);

  // Print instructions
  Serial.println("--- Incremental Calibration Mode ---");
  Serial.println("Commands: [Letter][Number]");
  Serial.println("b = Base | 1 = Joint 1 | 2 = Joint 2 | c = Clamp");
  Serial.println("Examples: b10 (Base +10 deg) | 1-5 (Joint1 -5 deg)");
  Serial.println("------------------------------------");
}

void loop() {
  // Check if data is available in the Serial Monitor
  if (Serial.available() > 0) {
    
    // Read the first character to determine the servo
    char cmd = Serial.read();
    
    // Ignore newline and carriage return characters
    if (cmd == '\n' || cmd == '\r') {
      return; 
    }

    // Parse the integer that comes immediately after the letter
    int amount = Serial.parseInt(); 

    // Route the command to the correct servo
    if (cmd == 'b' || cmd == 'B') {
      basePos = constrain(basePos + amount, 0, 180); // constrain prevents going past physical limits
      baseServo.write(basePos);
      Serial.print("Base adjusted to: ");
      Serial.println(basePos);
    } 
    else if (cmd == '1') {
      j1Pos = constrain(j1Pos + amount, 0, 180);
      joint1Servo.write(j1Pos);
      Serial.print("Joint 1 adjusted to: ");
      Serial.println(j1Pos);
    } 
    else if (cmd == '2') {
      j2Pos = constrain(j2Pos + amount, 0, 180);
      joint2Servo.write(j2Pos);
      Serial.print("Joint 2 adjusted to: ");
      Serial.println(j2Pos);
    } 
    else if (cmd == 'c' || cmd == 'C') {
      clampPos = constrain(clampPos + amount, 0, 180);
      clampServo.write(clampPos);
      Serial.print("Clamp adjusted to: ");
      Serial.println(clampPos);
    } 
    else {
      Serial.println("Invalid command. Use b, 1, 2, or c.");
    }
  }
}
