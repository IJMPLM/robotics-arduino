#include <Servo.h>

// --- SERVO PINS ---
#define servo1Pin 9 // Up-Down
#define servo2Pin 8 // Left-Right (Base)
#define servo3Pin 7 // Forward-Back
#define servo4Pin 6 // Clamp

// --- COLOR SENSOR PINS ---
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define sensorOut 10

Servo mservo1, mservo2, mservo3, mservo4;

// =======================================================
// CONFIGURATION ZONE: Adjust these based on your physical setup
// =======================================================

// 1. Container Drop-off Locations (Base Servo Angles)
int locationRed   = 45;   // Base angle for Red container
int locationGreen = 90;   // Base angle for Green container
int locationBlue  = 135;  // Base angle for Blue container

// 2. Arm Positions
int baseQueuePos = 0;     // The default Left-Right angle where the queue is
int armUpPos = 100;       // Up-Down angle for moving safely above containers
int armDownPos = 150;     // Up-Down angle to reach the object/container bottom
int clampOpenPos = 0;     // Clamp open
int clampClosePos = 35;   // Clamp closed

// 3. Color Sensor Calibration (From your previous tests)
int minRed = 50;   int maxRed = 600;
int minGreen = 50; int maxGreen = 600;
int minBlue = 50;  int maxBlue = 600;

// Object Detection Threshold (How high mapped RGB must be to trigger)
int objectThreshold = 100; 

// =======================================================

void setup() {
  Serial.begin(9600);
  
  // Attach servos
  mservo1.attach(servo1Pin); 
  mservo2.attach(servo2Pin); 
  mservo3.attach(servo3Pin); 
  mservo4.attach(servo4Pin); 

  // Initialize Color Sensor Pins
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Set Sensor Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Set Default Robot Posture
  mservo1.write(armUpPos); 
  mservo2.write(baseQueuePos); 
  mservo3.write(80); // Default forward-back
  mservo4.write(clampOpenPos); 
  
  Serial.println("System Ready. Waiting for objects...");
}

void loop() {
  // 1. Check what color is in the queue
  char detectedColor = readColorSensor();

  // 2. If a valid color is found, execute the sequence
  if (detectedColor != 'N') { // 'N' means None/No object
    Serial.print("Object Detected! Color: ");
    Serial.println(detectedColor);
    
    // Determine the target angle based on the color
    int targetAngle = 0;
    if (detectedColor == 'R') targetAngle = locationRed;
    else if (detectedColor == 'G') targetAngle = locationGreen;
    else if (detectedColor == 'B') targetAngle = locationBlue;

    // Execute the robotic sequence (This blocks new readings until finished)
    executePickAndPlace(targetAngle);
    
    // Small delay after returning to allow a new object to slide into place
    delay(2000); 
    Serial.println("Ready for next object.");
  }
}

// =======================================================
// FUNCTION: Smooth Servo Movement
// Prevents the robotic arm from jerking and throwing the object
// =======================================================
void slowMove(Servo &servo, int targetPos) {
  int currentPos = servo.read(); // Get where the servo currently is
  
  if (currentPos < targetPos) {
    for (int p = currentPos; p <= targetPos; p++) {
      servo.write(p);
      delay(20); // Speed of movement (higher = slower)
    }
  } else {
    for (int p = currentPos; p >= targetPos; p--) {
      servo.write(p);
      delay(20); 
    }
  }
}

// =======================================================
// FUNCTION: The Pick and Place Sequence
// =======================================================
void executePickAndPlace(int destinationAngle) {
  Serial.println("Sequence: Picking up...");
  
  // 1. Ensure Clamp is open and base is at Queue
  slowMove(mservo4, clampOpenPos); 
  slowMove(mservo2, baseQueuePos); 
  
  // 2. Lower the arm to pick
  slowMove(mservo1, armDownPos);
  delay(300); // Wait to stabilize
  
  // 3. Close Clamp
  slowMove(mservo4, clampClosePos);
  delay(500); // Ensure a firm grip
  
  Serial.println("Sequence: Moving to container...");
  
  // 4. Raise the arm back up
  slowMove(mservo1, armUpPos);
  
  // 5. Rotate base to the assigned color container
  slowMove(mservo2, destinationAngle);
  delay(300);
  
  Serial.println("Sequence: Dropping object...");
  
  // 6. Lower the arm into the container
  slowMove(mservo1, armDownPos);
  delay(300);
  
  // 7. Open clamp to drop
  slowMove(mservo4, clampOpenPos);
  delay(500);
  
  Serial.println("Sequence: Returning to start...");
  
  // 8. Raise arm back up
  slowMove(mservo1, armUpPos);
  
  // 9. Return base to the queue
  slowMove(mservo2, baseQueuePos);
}

// =======================================================
// FUNCTION: Read and Determine Color
// =======================================================
char readColorSensor() {
  // Read Red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  int rRaw = pulseIn(sensorOut, LOW);
  int rRGB = constrain(map(rRaw, minRed, maxRed, 255, 0), 0, 255);
  delay(50);

  // Read Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  int gRaw = pulseIn(sensorOut, LOW);
  int gRGB = constrain(map(gRaw, minGreen, maxGreen, 255, 0), 0, 255);
  delay(50);

  // Read Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  int bRaw = pulseIn(sensorOut, LOW);
  int bRGB = constrain(map(bRaw, minBlue, maxBlue, 255, 0), 0, 255);
  delay(50);

  // Logic to determine if an object is present and what color it is
  if (rRGB > objectThreshold || gRGB > objectThreshold || bRGB > objectThreshold) {
    if (rRGB > gRGB && rRGB > bRGB) {
      return 'R'; // Red is dominant
    } else if (gRGB > rRGB && gRGB > bRGB) {
      return 'G'; // Green is dominant
    } else if (bRGB > rRGB && bRGB > gRGB) {
      return 'B'; // Blue is dominant
    }
  }
  
  // If no color breaks the threshold, return N for None
  return 'N'; 
}