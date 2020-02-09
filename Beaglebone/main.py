import time
from SerialHandler import *

serialHandler = SerialHandler()

while True:
    for i in range(0, 361):

        # Measurement command test
        print("Requesting measurement...")
        distance = serialHandler.getDistance(i)
        if(distance < 0):
            print("Error! No response from Arduino!")
        else:
            print(distance)
        time.sleep(1)
        
        # Movement command test
        print("Sending Movement...")
        res = serialHandler.sendMovement(1.0)
        if(res):
            print("Success!")
        else:
            print("Failed!")
        
        # Rotation command test
        print("Sending rotation...")
        res = serialHandler.sendMovement(1.0)
        if(res):
            print("Success!")
        else:
            print("Failed!")
