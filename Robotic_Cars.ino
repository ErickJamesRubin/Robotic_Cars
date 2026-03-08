// --- MOTOR PINS ---
const int ENA = 10; const int IN1 = 9; const int IN2 = 8;
const int IN3 = 7;  const int IN4 = 6; const int ENB = 5;

// --- 3 IR SENSORS ---
const int irLeft = A0;  
const int irMiddle = A2; 
const int irRight = A1;

// --- MEMORY VARIABLE ---
int lastSide = 0; 

void setup() {
  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(irLeft, INPUT); pinMode(irMiddle, INPUT); pinMode(irRight, INPUT);
}

void loop() {
  int L = digitalRead(irLeft);
  int M = digitalRead(irMiddle);
  int R = digitalRead(irRight);

  // 1. CENTERED: Middle sensor is on tape
  if (M == 1) {
    moveForward();
  }
  // 2. DRIFTING RIGHT: Left sensor hits tape
  else if (L == 1 && R == 0) {
    lastSide = 1; // Update memory: Tape is on the Left
    moveLeft();
  }
  // 3. DRIFTING LEFT: Right sensor hits tape
  else if (R == 1 && L == 0) {
    lastSide = 2; // Update memory: Tape is on the Right
    moveRight();
  }
  // 4. TOTALLY LOST: (0,0,0)
  else {
    recoverToLine(); 
  }
}

void recoverToLine() {
  if (lastSide == 1) {
    // We last saw tape on the Left, so turn Left to find it
    moveLeft(); 
  } 
  else if (lastSide == 2) {
    // We last saw tape on the Right, so turn Right to find it
    moveRight();
  }
  else {
    // If we have NO memory (like at the very start), move backward slowly
    moveBackward();
  }
}

// --- MOVEMENT FUNCTIONS (Proven Speeds) ---

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