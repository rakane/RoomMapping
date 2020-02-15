import requests
import json

class HTTPHandler:
    def __init__(self, host, port):
        self.url = "http://" + host + ":" + port + "/upload"
        print(self.url)

    def sendMap(self, data):
        # Serialize data
        print(data)
        payload = {'map': data}
        serialData = json.dumps(payload)
        x = requests.post(self.url, data=serialData)
        res = json.loads(x.text) 
        print(res)

