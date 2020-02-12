#include "PacketSerial.h"
#include <Servo.h>

#define DIR_CW 0
#define DIR_ACW 1

// Communication
void onPacketReceived(const uint8_t* buffer, size_t size);

PacketSerial_<COBS> myPacketSerial;

typedef union _packet {
  float packet_float;
  uint8_t packet_data[4];
};

int currentOperation = -1;

// Servo Control
Servo myservo;

uint16_t fullyCW = 1000; //ACW meaning anti-clockwise (counter clockwise)
uint16_t fullyACW = 2000;

uint8_t servoControlPin = 9;
uint8_t servoFeedbackPin = 5;

int feedbackLow = 33;
int feedbackHigh = 1050;
float commandPos = 180.f;
float errorMargin = .6f;
int microsecondRange = 1000;
int microsecondMin = 1000;
int microsecondCenter = 1500;
int microsecondMax = 2000;
int minpulse = 45;
float currentPos = 0.f;
int pulseinVal = 0;

//Meta Values
int totalRange = feedbackHigh - feedbackLow;
float pulsePerDeg = totalRange/360.f;

int servoMoveHoming(float _commandPos);
float feedback2Deg(float _inputPulse);
int servoMoveCW(float _commandPos);
int servoMoveACW(float _commandPos);



void setup() {
  pinMode(servoFeedbackPin, INPUT);
  myservo.attach(servoControlPin);  // attaches the servo on pin 9 to the servo object
  pulseinVal = pulseIn(servoFeedbackPin, HIGH);
  currentPos = feedback2Deg(pulseinVal);
  Serial.begin(9600);
  Serial2.begin(9600);
  myPacketSerial.setStream(&Serial2);
  myPacketSerial.setPacketHandler(&onPacketReceived);
  
// Stop on 89 - 95
// 88 - X CW
// 96 - X CCW  
//  sensorServo.attach(sensorServoPin);
//  sensorServo.write(96);
}

void loop() {
  myPacketSerial.update();

  if (myPacketSerial.overflow()) {
    Serial.println("Buffer Overflow!");
  }
}


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
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }
    Serial.print("Angle to measure: ");
    Serial.println(incomingPacket.packet_float);
    servoMoveHoming((int) incomingPacket.packet_float);
    myPacketSerial.send(incomingPacket.packet_data, 4);
  }
  else if (currentOperation == 1)
  {
    Serial.println("Current Operation: Move");
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }
    Serial.print("Distance: ");
    Serial.println(incomingPacket.packet_float);

    _packet sendPacket;
    sendPacket.packet_float = 1.0;
    myPacketSerial.send(sendPacket.packet_data, 4);
  }
  else if (currentOperation == 2)
  {
    Serial.println("Current Operation: Rotate");
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }
    Serial.print("Angle to Rotate: ");
    Serial.println(incomingPacket.packet_float);

    _packet sendPacket;
    sendPacket.packet_float = 1.0;
    myPacketSerial.send(sendPacket.packet_data, 4);
  }
  Serial.println("---------------------------------------");
}

float feedback2Deg(float _inputPulse){
  float outDeg =  (_inputPulse-feedbackLow)/pulsePerDeg;
  return outDeg;
}

int servoMoveHoming(float _commandPos){

  //Current Position
  pulseinVal = pulseIn(servoFeedbackPin, HIGH);
  currentPos = feedback2Deg(pulseinVal);

  while(abs(_commandPos - currentPos) > errorMargin){
    //Determine which direction to rotate

    //Create copies of our actual values
    float limitedCommandPos = _commandPos;
    float limitedCurrentPos = currentPos;

    //Give them floors and cielings for the sake of repeatibility
    if(limitedCommandPos < 0.f){
      limitedCommandPos = 0.f;
    } else if(limitedCommandPos > 360.f) {
      limitedCommandPos = 360.f;
    }

    if(limitedCurrentPos < 0.f){
      limitedCurrentPos = 0.f;
    } else if(limitedCurrentPos > 360.f) {
      limitedCurrentPos = 360.f;
    }

    //Determine if the shortest path is going to be clockwise or counter clockwise
    if(limitedCommandPos > limitedCurrentPos){
      if((limitedCommandPos - limitedCurrentPos) > 180.f){
        servoMove(_commandPos, DIR_ACW);
      } else {
        servoMove(_commandPos, DIR_CW);
      }
    } else {
      if((limitedCurrentPos - limitedCommandPos) < 180.f){
        servoMove(_commandPos, DIR_ACW);
      } else {
        servoMove(_commandPos, DIR_CW);
      }
    }

    pulseinVal = pulseIn(servoFeedbackPin, HIGH);
    currentPos = feedback2Deg(pulseinVal);
  }
  
  
  return 0;
}

int servoMove(float _commandPos, int _dir){
  do{

    //Get the current position of the servo horn
    pulseinVal = pulseIn(servoFeedbackPin, HIGH);
    currentPos = feedback2Deg(pulseinVal);

    //Formulate a magnitude based upon how farway the horn is away from its desired position
    float absMag = (abs(currentPos - _commandPos) / 360.f) * (microsecondRange/2.f);

    //Let's set the speed based on how big our magnitude is. The resulting effect should be that if there is
    //a lot of distance the servo should move fast, but smaller distances it will go slower. This code puts
    //a floor and a cieling on the servo speed.
    int force = 0;
    if(DIR_CW == _dir){

      force = microsecondCenter - absMag;
      if(force < microsecondMin){
        force = microsecondMin;
      } else if(force > microsecondCenter - minpulse){
        force = microsecondCenter - minpulse;
      }

    } else {

      force = microsecondCenter + absMag;
      if(force > microsecondMax){
        force = microsecondMax;
      } else if(force < microsecondCenter + minpulse){
        force = microsecondCenter + minpulse;
      }
    }

    //We move the servo
    myservo.writeMicroseconds(force);
    
  }while(abs(currentPos - _commandPos) > errorMargin);

  myservo.writeMicroseconds(microsecondCenter);
  
  return 0;
}
