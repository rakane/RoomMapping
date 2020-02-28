#define LEFT_MOTOR 1
#define RIGHT_MOTOR 2
#define DEV_MODE 1

int motorLForward = 11;
int motorLBackward = 12;
int motorRForward = 4;
int motorRBackward = 10;
int leftIRSensor = 2;
int rightIRSensor = 3;
int obstacleLeft = 0;
int obstacleRight = 0;

void setup() {
  //  Serial.begin(9600);
  // Initialize output pins
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  pinMode(leftIRSensor, INPUT_PULLUP);
  pinMode(rightIRSensor, INPUT_PULLUP);

  // Make sure motors start off
  off();

  // Attach interrupt pins to IR sensors
  attachInterrupt(digitalPinToInterrupt(leftIRSensor), avoidLeft, LOW);
  attachInterrupt(digitalPinToInterrupt(rightIRSensor), avoidRight, LOW);

  if (DEV_MODE == 0) {
    forwards(LEFT_MOTOR);
    forwards(RIGHT_MOTOR);
  } else {
    off();
  }
}

void loop() {
  if (DEV_MODE == 1) {
    // DO NOTHING
    // Helpful so car doesnt drive off desk!!!
  } else {
    if (obstacleLeft == 1) {
      rotateRight(800);
    } else if (obstacleRight == 1) {
      rotateLeft(800);
    } else {
      forwards(LEFT_MOTOR);
      forwards(RIGHT_MOTOR);
    }
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

void off() {
  digitalWrite(motorLForward, LOW);
  digitalWrite(motorLBackward, LOW);
  digitalWrite(motorRForward, LOW);
  digitalWrite(motorRBackward, LOW);
}

void rotateLeft(int angle) {
  digitalWrite(motorLForward, HIGH);
  digitalWrite(motorRForward, LOW);
  digitalWrite(motorLBackward, LOW);
  digitalWrite(motorRBackward, HIGH);
  // This delay will control angle of movement until gyroscope implemented
  delay(angle);
  //  Serial.println("Returned to forwards");
  obstacleRight = 0;
}

void rotateRight(int angle) {
  digitalWrite(motorLForward, LOW);
  digitalWrite(motorRForward, HIGH);
  digitalWrite(motorLBackward, HIGH);
  digitalWrite(motorRBackward, LOW);
  // This delay will control angle of movement until gyroscope implemented
  delay(angle);
  //  Serial.println("Returned to forwards");
  obstacleLeft = 0;
}

void avoidRight() {
  //  Serial.println("Obstacle on Right");
  if (obstacleLeft = 1) {

  } else {
    obstacleRight = 1;
  }
}

void avoidLeft() {
  //  Serial.println("Obstacle on Left");
  if (obstacleRight = 1) {

  } else {
    obstacleLeft = 1;
  }
}
