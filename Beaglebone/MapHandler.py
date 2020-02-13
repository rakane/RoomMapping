
class MapHandler:
    def __init__(self):
        self.roomMap = []
        for i in range(0, 36):
            self.roomMap.append(-1000)
        
    def initialMap(self, data):
        self.roomMap = data

    def updateValue(self, index, val):
        self.roomMap[index] = val

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
            print(self.roomMap[i], end=' ')

        print(self.roomMap[35],end=' ')
        print("]")
           
