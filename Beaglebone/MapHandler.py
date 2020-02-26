import matplotlib

matplotlib.use('Agg')

import matplotlib.pyplot as plt
import math

class MapHandler:
    def __init__(self):
        self.roomMap = []
        for i in range(0, 19):
            self.roomMap.append(-1000)
        
    def initialMap(self, data):
        self.roomMap = data
    
    def getMap(self):
        return self.roomMap

    def updateValue(self, index, val):
        
        self.roomMap[index - 5] = val

    def updateSection(self, startIndex, stopIndex, data, resolution):
        dataIndex = 0
        for i in range(startIndex, stopIndex + 1, resolution):
            self.roomMap[i] = data[dataIndex]
            dataIndex += 1

    def calculateRotation(self, angle):
        if angle > 180 or angle < -180:
            print("Invalid Angle")
            return -1

        if angle > 0:
            temp = []
            # Fill temp array with 0 - angle indices
            for i in range(0, angle):
                temp.append(self.roomMap[i])

            # Shift remainder of elements left
            for i in range (0, 359):
                self.roomMap[i] = self.roomMap[i + 1]
                
            tempIndex = 0
            # Fill back in temp array elements at end of map
            for i in range(359-angle, 360):
                self.mapRoom[i] = temp[tempIndex]
                tempIndex += 1
        else:
            temp = []
            # Fill temp array with (359-angle) - 359 indices
            for i in range(359-angle, 360):
                temp.append(roomMap[i])

            # Shift remainder of elements right
            for i in range(359, 1, -1):
                self.mapRoom[i] = self.mapRoom[i - 1]

            # Fill back in temp array elements at beginning of map
            for i in range(0, angle):
                self.mapRoom[i] = temp[i]
    
        return 1

    def print(self):
        print(" [ ", end='')
        for i in range(0, 35):
            print(self.roomMap[i], end=', ')

        print(self.roomMap[35],end=' ')
        print("]")

    def showPlot(self):
        theta = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110,
                120, 130, 140, 150, 160, 170, 180, 190, 200,
                210, 220, 230, 240, 250, 260, 270, 280, 290,
                300, 310, 320, 330, 340, 350 ]
        
        distance = self.roomMap

        # Convert degrees to radians
        for i in range(0, 36):
            theta[i] = theta[i] * (math.pi / 180)

        # Complete circle by making a 360 point equal to 0
        # Simply for visualization
        theta.append(theta[0])
        distance.append(distance[0])

        plt.polar(theta, distance)
        plt.show()


