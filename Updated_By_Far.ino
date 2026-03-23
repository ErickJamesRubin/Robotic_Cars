// -------- MOTOR PINS (L298N) --------
const int ENA = 10; const int IN1 = 9;  const int IN2 = 8;
const int IN3 = 7;  const int IN4 = 6;  const int ENB = 5;

// -------- IR SENSOR PINS --------
const int irLeft   = A0;
const int irMiddle = A2;
const int irRight  = A1;

// -------- ULTRASONIC PINS --------
#define TRIG_PIN 11
#define ECHO_PIN 12

// -------- SERVO PIN --------
#define SERVO_PIN 3

// -------- SETTINGS --------
int Set = 20;                  // obstacle distance threshold (cm)
int distance_L, distance_F, distance_R;
int lastSide = 0;              // IR memory: 1=left, 2=right

// ===================================================
void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);

  // IR sensors
  pinMode(irLeft,   INPUT);
  pinMode(irMiddle, INPUT);
  pinMode(irRight,  INPUT);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Servo
  pinMode(SERVO_PIN, OUTPUT);

  // Servo sweep on startup (center at 70 degrees)
  for (int a = 70; a <= 140; a += 5) servoPulse(SERVO_PIN, a);
  for (int a = 140; a >= 0;  a -= 5) servoPulse(SERVO_PIN, a);
  for (int a = 0;  a <= 70;  a += 5) servoPulse(SERVO_PIN, a);

  distance_F = Ultrasonic_read();
  delay(500);
}

// ===================================================
void loop() {

  distance_F = Ultrasonic_read();
  Serial.print("D F="); Serial.println(distance_F);

  // =========================
  // OBSTACLE CHECK (PRIORITY 1)
  // =========================
  if (distance_F < Set) {
    Check_side();
    return;  // skip IR logic this cycle
  }

  // =========================
  // LINE FOLLOWING (PRIORITY 2)
  // =========================
  int L = digitalRead(irLeft);
  int M = digitalRead(irMiddle);
  int R = digitalRead(irRight);

  if (M == 1) {
    // Centered on line
    Serial.println("Forward");
    moveForward();
  }
  else if (L == 1 && R == 0) {
    // Drifting right — tape seen on left side
    lastSide = 1;
    Serial.println("Correct Left");
    moveLeft();
  }
  else if (R == 1 && L == 0) {
    // Drifting left — tape seen on right side
    lastSide = 2;
    Serial.println("Correct Right");
    moveRight();
  }
  else {
    // All sensors lost (0,0,0) — use memory to recover
    recoverToLine();
  }

  delay(5);
}

// ===================================================
// LINE RECOVERY
// ===================================================
void recoverToLine() {
  if (lastSide == 1) {
    Serial.println("Recover: turn Left");
    moveLeft();
  }
  else if (lastSide == 2) {
    Serial.println("Recover: turn Right");
    moveRight();
  }
  else {
    Serial.println("No memory: Backward");
    moveBackward();
  }
}

// ===================================================
// MOTOR FUNCTIONS
// ===================================================
void moveForward() {
  analogWrite(ENA, 170); analogWrite(ENB, 170);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveLeft() {
  analogWrite(ENA, 200); analogWrite(ENB, 200);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveRight() {
  analogWrite(ENA, 200); analogWrite(ENB, 200);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void moveBackward() {
  analogWrite(ENA, 200); analogWrite(ENB, 200);
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void Stop() {
  analogWrite(ENA, 0); analogWrite(ENB, 0);
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// ===================================================
// SERVO CONTROL
// ===================================================
void servoPulse(int pin, int angle) {
  int pwm = (angle * 11) + 500;
  digitalWrite(pin, HIGH);
  delayMicroseconds(pwm);
  digitalWrite(pin, LOW);
  delay(30);
}

// ===================================================
// ULTRASONIC SENSOR
// ===================================================
long Ultrasonic_read() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long t = pulseIn(ECHO_PIN, HIGH);
  return t / 29 / 2;
}

// ===================================================
// OBSTACLE AVOIDANCE
// ===================================================
void Check_side() {
  Stop();
  delay(100);

  // Look RIGHT
  for (int a = 70; a <= 140; a += 5) servoPulse(SERVO_PIN, a);
  delay(300);
  distance_R = Ultrasonic_read();
  Serial.print("D R="); Serial.println(distance_R);
  delay(100);

  // Look LEFT
  for (int a = 140; a >= 0; a -= 5) servoPulse(SERVO_PIN, a);
  delay(500);
  distance_L = Ultrasonic_read();
  Serial.print("D L="); Serial.println(distance_L);
  delay(100);

  // Return to center
  for (int a = 0; a <= 70; a += 5) servoPulse(SERVO_PIN, a);
  delay(300);

  compareDistance();
}

void compareDistance() {
  if (distance_L > distance_R) {
    // More space on the left — go around left
    moveLeft();    delay(500);
    moveForward(); delay(400);
    moveRight();   delay(500);
    moveForward(); delay(500);
    moveRight();   delay(400);
    moveForward(); delay(420);
  }
  else {
    // More space on the right — go around right
    moveRight();   delay(500);
    moveForward(); delay(400);
    moveLeft();    delay(500);
    moveForward(); delay(500);
    moveLeft();    delay(400);
    moveForward(); delay(420);
  }
}