#include "PacketSerial.h"

PacketSerial_<COBS> myPacketSerial;

typedef union _packet {
  float packet_float;
  uint8_t packet_data[4];
};

_packet myPacket;

void onPacketReceived(const uint8_t* buffer, size_t size);

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

  for(int i = 0; i < 4; i++) {
      incomingPacket.packet_data[i] = tempBuffer[i];
  }
  
  Serial.println(incomingPacket.packet_float);
  myPacketSerial.send(incomingPacket.packet_data, 4);

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
