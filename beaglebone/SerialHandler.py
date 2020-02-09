import Adafruit_BBIO.UART as UART
import serial
from cobs import cobs
import struct
import time

class SerialHandler:
    def __init__(self):
        UART.setup("UART1");
        self.ser = serial.Serial(port="/dev/ttyS1", baudrate = 9600)
        self.ser.close()
        self.ser.open()
        self.ser.isOpen()
        print("Serial communication ready!")

    def getDistance(self, angle):
        encoded = cobs.encode(struct.pack("f", angle))
        self.ser.write(encoded + b"\00")
         

        dataBuffer = b""
        startTime = int(round(time.time() * 1000))
        currentTime = startTime
        while (currentTime - startTime) < 3000:
            if self.ser.inWaiting() > 0:
                val = self.ser.read(1)
                if val == b'\x00':
                    data = cobs.decode(dataBuffer)
                    unpackedData = struct.unpack('f', data)[0]
                    return unpackedData
                    dataBuffer = b""
                else:
                    dataBuffer += val

            currentTime = int(round(time.time() * 1000))
   
        return -1
