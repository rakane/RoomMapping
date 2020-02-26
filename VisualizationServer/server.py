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

    theta = [180, 170, 160, 150, 140, 130, 120, 
            110, 100, 90, 80, 70, 60, 50, 40, 30, 
            20, 10, 0 ]

    # Convert degrees to radians
    for i in range(0, 19):
        theta[i] = theta[i] * (math.pi / 180)

    unwantedTheta = [];
    unwantedDistances = {-1000}

    for i in range(0, 19):
        if distances[i] == -1000:
            unwantedTheta.append(theta[i])

    newThetas = [ele for ele in theta if ele not in unwantedTheta]
    newDistances = [ele for ele in distances if ele not in unwantedDistances]

    newThetas.append(newThetas[0])
    newDistances.append(newDistances[0])
  

    plt.clf()
    plt.polar(newThetas, newDistances)
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

