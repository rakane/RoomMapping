#include "PacketSerial.h"
#include <Servo.h>

#define DIR_CW 0
#define DIR_CCW 1

// Communication
PacketSerial_<COBS> myPacketSerial;

typedef union _packet {
  float packet_float;
  uint8_t packet_data[4];
};

_packet errorPacket;

int currentOperation = -1;
// End Communication

// Sensor Servo
Servo myservo;

uint16_t fullyCW = 1000;    // Counter-Clockwise
uint16_t fullyCCW = 2000;   // Clockwise

uint8_t servoControlPin = 9;
uint8_t servoFeedbackPin = 5;

int feedbackLow = 33;
int feedbackHigh = 1050;
float commandPos = 180.f;
float errorMargin = 0.6f;
int microsecondRange = 1000;
int microsecondMin = 1000;
int microsecondCenter = 1500;
int microsecondMax = 2000;
int minpulse = 45;
float currentPos = 0.f;
int pulseinVal = 0;

int totalRange = feedbackHigh - feedbackLow;
float pulsePerDeg = totalRange / 360.f;
// End Sensor Servo

// Ultrasonic Sensor
uint8_t pingPin = 7;
uint8_t echoPin = 6;
// End Ultrasonic Sensor

// Functions
void onPacketReceived(const uint8_t* buffer, size_t size);
int servoMoveHoming(float _commandPos);
float feedback2Deg(float _inputPulse);
int servoMoveCW(float _commandPos);
int servoMoveACW(float _commandPos);
float getDistance();

void setup() {
  // Ultrasonic Sensor Setup
  pinMode(pingPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Servo setup
  pinMode(servoFeedbackPin, INPUT);
  myservo.attach(servoControlPin);

  // Servo initial position
  pulseinVal = pulseIn(servoFeedbackPin, HIGH);
  currentPos = feedback2Deg(pulseinVal);

  // Communication setup
  Serial.begin(9600);
  Serial2.begin(9600);
  myPacketSerial.setStream(&Serial2);
  myPacketSerial.setPacketHandler(&onPacketReceived);
  errorPacket.packet_float = -1;

  // Stop on 89 - 95
  // 88 - X CW
  // 96 - X CCW
  //  sensorServo.attach(sensorServoPin);
  //  sensorServo.write(96);
}

void loop() {

  // Check for incoming packets
  myPacketSerial.update();

  // Check for buffer overflow
  if (myPacketSerial.overflow()) {
    Serial.println("Buffer Overflow!");
  }
}


// Handles communication with Beaglbone
void onPacketReceived(const uint8_t* buffer, size_t size) {
  uint8_t tempBuffer[size];
  memcpy(tempBuffer, buffer, size);
  _packet incomingPacket;

  // Handle which command is going to be operated with the data that will follow
  if (tempBuffer[0] == 100) {
    currentOperation = 0;
    return;
  } else if (tempBuffer[0] == 109) {
    currentOperation = 1;
    return;
  } else if (tempBuffer[0] == 114) {
    currentOperation = 2;
    return;
  }


  // Perform operation with data sent after previous command
  Serial.println("---------------------------------------");
  if (currentOperation == 0)
  {
    Serial.println("Current Operation: Gather Data");

    // Convert uint8_t array to float
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }
    Serial.print("Angle to measure: ");
    Serial.println(incomingPacket.packet_float);

    // Rotate sensor servo motor
    int status = servoMoveHoming((int) incomingPacket.packet_float);

    // if status = -1, then an error occured and data wasn't retrieved
    if (status == -1) {
      myPacketSerial.send(errorPacket.packet_data, 4);
    } else {
      _packet responsePacket;
      responsePacket.packet_float = getDistance();
      myPacketSerial.send(responsePacket.packet_data, 4);
    }
  }
  else if (currentOperation == 1)
  {
    Serial.println("Current Operation: Move");

    // Convert uint8_t array to float
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }

    Serial.print("Distance: ");
    Serial.println(incomingPacket.packet_float);

    // Send response packet
    _packet sendPacket;
    sendPacket.packet_float = 1.0;
    myPacketSerial.send(sendPacket.packet_data, 4);
  }
  else if (currentOperation == 2)
  {
    Serial.println("Current Operation: Rotate");

    // Convert uint8_t array to float
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }

    Serial.print("Angle to Rotate: ");
    Serial.println(incomingPacket.packet_float);

    // Send response packet
    _packet sendPacket;
    sendPacket.packet_float = 1.0;
    myPacketSerial.send(sendPacket.packet_data, 4);
  }
  Serial.println("---------------------------------------");
}

// Calculates Servo motor angle from feedback
float feedback2Deg(float _inputPulse) {
  float outDeg =  (_inputPulse - feedbackLow) / pulsePerDeg;
  return outDeg;
}

// Handles angle calculations for servo rotations
int servoMoveHoming(float _commandPos) {

  // Get current Servo position
  pulseinVal = pulseIn(servoFeedbackPin, HIGH);
  currentPos = feedback2Deg(pulseinVal);

  // Stay in loop until error is acceptable
  while (abs(_commandPos - currentPos) > errorMargin) {
    //Determine which direction to rotate

    //Create copies of our actual values
    float limitedCommandPos = _commandPos;
    float limitedCurrentPos = currentPos;

    //Give them floors and cielings for the sake of repeatibility
    if (limitedCommandPos < 0.f) {
      limitedCommandPos = 0.f;
    } else if (limitedCommandPos > 360.f) {
      limitedCommandPos = 360.f;
    }

    if (limitedCurrentPos < 0.f) {
      limitedCurrentPos = 0.f;
    } else if (limitedCurrentPos > 360.f) {
      limitedCurrentPos = 0.f;
    }

    if (limitedCommandPos < limitedCurrentPos) {
      int status = servoMove(_commandPos, DIR_CCW);

      // Status = -1 means an error occured
      if (status == -1) {
        return -1;
      }
    } else {
      int status = servoMove(_commandPos, DIR_CW);

      // Status = -1 means an error occured
      if (status == -1) {
        return -1;
      }
    }


    //Determine if the shortest path is going to be clockwise or counter clockwise
    //    if(limitedCommandPos > limitedCurrentPos){
    //      if((limitedCommandPos - limitedCurrentPos) > 180.f){
    //        servoMove(_commandPos, DIR_ACW);
    //      } else {
    //        servoMove(_commandPos, DIR_CW);
    //      }
    //    } else {
    //      if((limitedCurrentPos - limitedCommandPos) < 180.f){
    //        servoMove(_commandPos, DIR_ACW);
    //      } else {
    //        servoMove(_commandPos, DIR_CW);
    //      }
    //    }

    // Calculate new positions
    pulseinVal = pulseIn(servoFeedbackPin, HIGH);
    currentPos = feedback2Deg(pulseinVal);
  }


  return 0;
}

int servoMove(float _commandPos, int _dir) {
  // Timeout control variables
  unsigned long startTime = millis();
  unsigned long currTime = millis();

  do {
    // Calculate current Servo position
    pulseinVal = pulseIn(servoFeedbackPin, HIGH);
    currentPos = feedback2Deg(pulseinVal);

    // Formulate a magnitude based upon how farway the Servo is away from its desired position
    float absMag = (abs(currentPos - _commandPos) / 360.f) * (microsecondRange / 2.f);

    // Let's set the speed based on how big our magnitude is. The resulting effect should be that if there is
    // a lot of distance the servo should move fast, but smaller distances it will go slower. This code puts
    // a floor and a ceiling on the servo speed.
    int force = 0;
    if (DIR_CW == _dir) {

      // Add floor and ceiling check
      force = microsecondCenter - absMag;
      if (force < microsecondMin) {
        force = microsecondMin;
      } else if (force > microsecondCenter - minpulse) {
        force = microsecondCenter - minpulse;
      }

    } else {

      // Add floor and ceiling check
      force = microsecondCenter + absMag;
      if (force > microsecondMax) {
        force = microsecondMax;
      } else if (force < microsecondCenter + minpulse) {
        force = microsecondCenter + minpulse;
      }
    }

    // We move the servo
    myservo.writeMicroseconds(force);

    currTime = millis();

    // If reducing the error below the acceptable level takes longer
    // than 2 seconds, break and return -1 indicating an error occured
    if (currTime - startTime > 2000) {
      Serial.print("Final Error: ");
      Serial.println(abs(currentPos - _commandPos));
      return -1;
    }
  } while (abs(currentPos - _commandPos) > errorMargin);

  // Stop Servo at correct position using center
  myservo.writeMicroseconds(microsecondCenter);

  return 0;
}


float getDistance() {
  long cms[10] = { -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000 };
  long duration;
  long sum = 0;
  float average;
  float cm;

  // Get 10 data samples
  for (int i = 0; i < 10; i++) {
    digitalWrite(pingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(pingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(pingPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    cms[i] = duration / 29.0 / 2.0;
    sum += cms[i];
  }

  average = sum / 5.0;

  // If any data came back as out of range, retry
  for (int i = 0; i < 10; i++) {
    if (cms[i] > 200) {
      digitalWrite(pingPin, LOW);
      delayMicroseconds(2);
      digitalWrite(pingPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(pingPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      cms[i] = duration / 29.0 / 2.0;
    }
  }

  // Remove outliers that are higher than average (better to undershoot and not collide)
  // Outlier threshold is arbitrary until testing
  for(int i = 0; i < 10; i++) {
    if(cms[i] - average > 50) {
      cms[i] = -1000;
    }
  }

  // Recalculate average
  int count = 0;
  sum = 0.0;
  for(int i = 0; i < 10; i++) {
    if(cms[i] != -1000) {
      sum += cms[i];
      count++;
    }
  }

  float finalAvg = sum / count;

  Serial.print("Average: ");
  Serial.print(finalAvg);
  Serial.print("cm");
  Serial.println();
  return finalAvg;
}
