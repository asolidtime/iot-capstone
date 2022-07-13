from flask import Flask
from array import *
import paho.mqtt.client as mqtt

leaderboard = []

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("lindenline/leaderboard")

def on_message(client, userdata, msg):
    score = float(msg.payload) / 1000
    leaderboard.append(score) # millis to seconds
    leaderboard.sort()
    client.publish("lindenline/leaderboardmsg", payload=f"#{leaderboard.index(score) + 1} - {score}")
    print(f"#{leaderboard.index(score) + 1} - {score}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("lab.thewcl.com", 1883, 60)

client.loop_start()

app = Flask(__name__)

@app.route('/')
def showLeaderboard():
    outputstring = "<h3>"
    leaderboard.sort()
    for listing in leaderboard:
        outputstring += "#"
        outputstring += str(leaderboard.index(listing) + 1)
        outputstring += " - "
        outputstring += str(listing)
        outputstring += "<br>"
    outputstring += "</h3>"
    return outputstring