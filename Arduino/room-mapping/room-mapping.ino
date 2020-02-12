#include "PacketSerial.h"
#include <Servo.h>

// Communication
void onPacketReceived(const uint8_t* buffer, size_t size);

PacketSerial_<COBS> myPacketSerial;

typedef union _packet {
  float packet_float;
  uint8_t packet_data[4];
};

int currentOperation = -1;

// Servo Control
Servo sensorServo;

uint8_t sensorServoPin = 9;
uint8_t feedbackPin = A0;

float tHigh = 0;
float tLow = 0;
int tCycle = 0;
float dc = 0;
float angle = 0; //Measured angle from feedback
float dcMin = 2.9; //From Parallax spec sheet
float dcMax = 97.1; //From Parallax spec sheet
float Kp = .4; //Proportional Gain, higher values for faster response, higher values contribute to overshoot.
float Ki = .2; //Integral Gain, higher values to converge faster to zero error, higher values produce oscillations. Higher values are more unstable near a target_angle = 0.
float iLimit = 5; //Arbitrary Anti-wind-up
float Kd = 1; //Derivative Gain, higher values dampen oscillations around target_angle. Higher values produce more holding state jitter. May need filter for error noise.
float prev_error = 0;
float prev_pError = 0;
float error = 0;
float pError = 0;
float iError = 0;


void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  myPacketSerial.setStream(&Serial2);
  myPacketSerial.setPacketHandler(&onPacketReceived);
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
    uint8_t status = moveToAngle((int) incomingPacket.packet_float);
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


uint8_t moveToAngle(int targetAngle) {
  sensorServo.attach(sensorServoPin);
  Serial.println(targetAngle);

  while (1) //From Parallax spec sheet
  {
    tHigh = pulseIn(feedbackPin, HIGH);
    tLow = pulseIn(feedbackPin, LOW);
    tCycle = tHigh + tLow;
    if ( tCycle > 1000 && tCycle < 1200)
    {
      break; //valid tCycle;
    }
  }

  dc = (100 * tHigh) / tCycle; //From Parallax spec sheet, you are trying to determine the percentage of the HIGH in the pulse

  angle = ((dc - dcMin) * 360) / (dcMax - dcMin + 1); //From Parallax spec sheet

  //Keep measured angles within bounds
  if (angle < 0)
  {
    angle = 0;
  }
  else if (angle > 359)
  {
    angle = 359;
  }

  if (targetAngle < 0)
  {
    targetAngle = 360 + targetAngle; //handles negative target_angles;
  }

  error = targetAngle - angle;

  if (error > 180)
  {
    error = error - 360; //tells it to rotate in the other direction because it is a smaller angle that way.
  }
  if (error < -180)
  {
    error = 360 - error - 360; //tells it to rotate in the other direction because it is a smaller angle that way.
  }

  // PID controller stuff, Adjust values of Kp, Ki, and Kd above to tune your system
  float pError = Kp * error;
  float iError = Ki * (error + prev_error);

  if  (iError > iLimit)
  {
    iError = iLimit;
  }
  if (iError <  -iLimit)
  {
    iError = -iLimit;
  }

  prev_error = error;
  float dError = Kd * (pError - prev_pError);
  prev_pError = pError;

  error = error / 2; //max 180 error will have max 90 offset value

  int val = 90 - (Kp * error) - iError - dError; // 93 is the middle of my servo's "no motion" dead-band

  sensorServo.write(val); // Move the servo
  delay(500);
  sensorServo.detach();
  return 1;
}
