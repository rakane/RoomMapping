import time
from SerialHandler import *
from MapHandler import *

serialHandler = SerialHandler()
mapHandler = MapHandler()

while True:
    i = 0;
    while i < 360:
        # Measurement command test
        print("------------------------------")
        print("Requesting measurement...")
        distance = serialHandler.getDistance(i)
        if(distance < 0):
            if(distance == -1):
                print("Error! Angle measurement invalid")
                distance = serialHandler.getDistance(0);
                i -= 10
            else:
                print("Error, no response from Arduino")
                serialHandler.resetSensorMotor();
                i -= 10
        else:
            print("Response Distance: " + str(distance))
            mapHandler.updateValue( i // 10, distance)
        print("------------------------------")
        time.sleep(0.2)
        i += 10

        ## Movement command test
        #print("------------------------------")
        #print("Sending Movement...")
        #res = serialHandler.sendMovement(1.0)
        #if(res):
        #    print("Success!")
        #else:
        #    print("Failed!")
        #print("------------------------------")
        #time.sleep(3)
    
        ## Rotation command test
        #print("------------------------------")
        #print("Sending rotation...")
        #res = serialHandler.sendRotation(3.0)
        #if(res):
        #    print("Success!")
        #else:
        #    print("Failed!")
        #print("------------------------------")
        #time.sleep(3)
    mapHandler.print()
