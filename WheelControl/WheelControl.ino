#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2
#define DEV_MODE 0

int motorLForward = 11;
int motorLBackward = 12;
int motorRForward = 4;
int motorRBackward = 10;
int leftIRSensor = 2;
int rightIRSensor = 3;
int interruptDelay = 0;

void setup() {
  Serial.begin(9600);
  // Initialize output pins
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  pinMode(leftIRSensor, INPUT);
  pinMode(rightIRSensor, INPUT);

  // Make sure motors start off
  off(LEFT_MOTOR);
  off(RIGHT_MOTOR);

  // Attach interrupt pins to IR sensors
  attachInterrupt(digitalPinToInterrupt(leftIRSensor), avoidLeft, LOW);
  attachInterrupt(digitalPinToInterrupt(rightIRSensor), avoidRight, LOW);
}

void loop() {
  Serial.println(digitalRead(leftIRSensor));

 
  if (DEV_MODE == 1) {
    // DO NOTHING
    // Helpful so car doesnt drive off desk!!! 
  } else {
    // Move backwards and forwards repetitvely
    forwards(LEFT_MOTOR);
    forwards(RIGHT_MOTOR);
    delay(1000);
    backwards(LEFT_MOTOR);
    backwards(RIGHT_MOTOR);
    delay(1000);
  }
}

void forwards(int motor) {
  if (motor == LEFT_MOTOR) {
    digitalWrite(motorLForward, HIGH);
    digitalWrite(motorLBackward, LOW);
  } else {
    digitalWrite(motorRForward, HIGH);
    digitalWrite(motorRBackward, LOW);
  }
}

void backwards(int motor) {
  if (motor == LEFT_MOTOR) {
    digitalWrite(motorLForward, LOW);
    digitalWrite(motorLBackward, HIGH);
  } else {
    digitalWrite(motorRForward, LOW);
    digitalWrite(motorRBackward, HIGH);
  }
}

void off(int motor) {
  if (motor == LEFT_MOTOR) {
    digitalWrite(motorLForward, LOW);
    digitalWrite(motorLBackward, LOW);
  } else {
    digitalWrite(motorRForward, LOW);
    digitalWrite(motorRBackward, LOW);
  }
}

void rotateLeft(int angle) {
  digitalWrite(motorLForward, LOW);
  digitalWrite(motorRForward, HIGH);
  digitalWrite(motorLBackward, HIGH);
  digitalWrite(motorRBackward, LOW);
  // This delay will control angle of movement until gyroscope implemented
  delay(angle);
  off(LEFT_MOTOR);
  off(RIGHT_MOTOR);
}

void rotateRight(int angle) {
  digitalWrite(motorLForward, HIGH);
  digitalWrite(motorRForward, LOW);
  digitalWrite(motorLBackward, LOW);
  digitalWrite(motorRBackward, HIGH);
  // This delay will control angle of movement until gyroscope implemented
  delay(angle);
  off(LEFT_MOTOR);
  off(RIGHT_MOTOR);
}

void avoidRight() {
  rotateLeft(500);
  interruptDelay = 1;
}

void avoidLeft() {
  rotateRight(500);
  interruptDelay = 1;
}
