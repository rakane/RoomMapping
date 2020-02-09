import time
from SerialHandler import *

serialHandler = SerialHandler()

while True:
    for i in range(0, 361):
        distance = serialHandler.getDistance(i)        
        if(distance < 0):
            print("Error! No response from Arduino!")
        else:
            print(distance)

        time.sleep(1)
