import matplotlib.pyplot as plt
import math
from flask import Flask, request, render_template, send_file
import json
import os

app = Flask(__name__)

@app.route("/upload", methods=['POST'])
def addPlot():
    data = json.loads(request.data)
    distances = data['map']
    print(distances)

    theta = [0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 
            120, 130, 140, 150, 160, 170, 180, 190, 200, 
            210, 220, 230, 240, 250, 260, 270, 280, 290, 
            300, 310, 320, 330, 340, 350 ]

    # Convert degrees to radians
    for i in range(0, 36):
        theta[i] = theta[i] * (math.pi / 180)
    
    # Remove any point at -1000 value
    # This means no value was detected in range, and is ingored for graph
    # Add indices in reverse order to delete from back of list first and no mess with length
    indicesToRemove = []
    for i in range(0,36):
        if(distances[i] < 0):
            indicesToRemove.insert(0, i)
    
    numRemoved = 0
    for i in range(0, len(indicesToRemove)):
        distances.remove(i - numRemoved)
        theta.remove(i - numRemoved)
        numRemoved += 1

    # Complete circle by making a 360 point equal to 0
    # Simply for visualization
    theta.append(theta[0])
    distances.append(distances[0])
    
    plt.clf()
    plt.polar(theta, distances)
    plt.savefig('./static/map.png')
    
    return json.dumps({'status': 1})

@app.route("/", methods=['GET'])
def showPlot():
    return render_template("index.html")

@app.route("/image", methods=['GET'])
def get_image():
    return send_file("./static/map.png", mimetype='image/gif')

@app.after_request
def add_header(response):
    # response.cache_control.no_store = True
    response.headers['Cache-Control'] = 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0, max-age=0'
    response.headers['Pragma'] = 'no-cache'
    response.headers['Expires'] = '-1'
    return response

if __name__ == "__main__":
    app.run(host='0.0.0.0')
    app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0

