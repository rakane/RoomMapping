import time
from SerialHandler import *
from MapHandler import *

serialHandler = SerialHandler()
mapHandler = MapHandler()

while True:
    for i in range(0, 361):

        # Measurement command test
        print("------------------------------")
        print("Requesting measurement...")
        distance = serialHandler.getDistance(i)
        if(distance < 0):
            print("Error! No response from Arduino!")
        else:
            print("Response Distance: " + str(distance))
        print("------------------------------")
        time.sleep(3)

        # Movement command test
        print("------------------------------")
        print("Sending Movement...")
        res = serialHandler.sendMovement(1.0)
        if(res):
            print("Success!")
        else:
            print("Failed!")
        print("------------------------------")
        time.sleep(3)
    
        # Rotation command test
        print("------------------------------")
        print("Sending rotation...")
        res = serialHandler.sendRotation(3.0)
        if(res):
            print("Success!")
        else:
            print("Failed!")
        print("------------------------------")
        time.sleep(3)

