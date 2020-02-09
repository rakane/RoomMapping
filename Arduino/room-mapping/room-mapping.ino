#include "PacketSerial.h"

PacketSerial_<COBS> myPacketSerial;

typedef union _packet {
  float packet_float;
  uint8_t packet_data[4];
};

_packet myPacket;

void onPacketReceived(const uint8_t* buffer, size_t size);

int currentOperation = -1;


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  myPacketSerial.setStream(&Serial2);
  myPacketSerial.setPacketHandler(&onPacketReceived);
  myPacket.packet_float = 100.0;
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


  Serial.println("---------------------------------------");
  if (currentOperation == 0) 
  {
    Serial.println("Current Operation: Gather Data");
    for (int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
    }
    Serial.print("Angle to measure: ");
    Serial.println(incomingPacket.packet_float);
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

void printPacket(_packet packet) {
  Serial.print("Float: ");
  Serial.println(packet.packet_float);
  Serial.print("Char Array: ");
  Serial.print(packet.packet_data[0]);
  Serial.print(" ");
  Serial.print(packet.packet_data[1]);
  Serial.print(" ");
  Serial.print(packet.packet_data[2]);
  Serial.print(" ");
  Serial.println(packet.packet_data[3]);
  Serial.print("Char Array Hex: ");
  Serial.print(packet.packet_data[0], HEX);
  Serial.print(" ");
  Serial.print(packet.packet_data[1], HEX);
  Serial.print(" ");
  Serial.print(packet.packet_data[2], HEX);
  Serial.print(" ");
  Serial.println(packet.packet_data[3], HEX);
  Serial.println("-----------------------------");
}
