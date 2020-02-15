import time
from SerialHandler import *
from MapHandler import *
from HTTPHandler import *

plotHost = '192.168.1.11'
plotPort = '5000'

serialHandler = SerialHandler()
mapHandler = MapHandler()
httpHandler = HTTPHandler(plotHost, plotPort)

while True:
    i = 0;
    while i < 360:
        # Measurement command test
        print("------------------------------")
        print("Requesting measurement at angle of", end=' ')
        print(i, end='')
        print("...")
        distance = serialHandler.getDistance(i)
        if(distance < 0):
            if(distance == -1):
                print("Error! Angle measurement invalid")
                serialHandler.resetSensorMotor()
                i -= 10
            else:
                print("Error, no response from Arduino")
                exit(1)
                i -= 10
        else:
            print("Response Distance: " + str(distance))
            if(distance > 200):
                mapHandler.updateValue( i // 10, -1000)
            else:
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
    # mapHandler.print()
    httpHandler.sendMap(mapHandler.getMap()) 

