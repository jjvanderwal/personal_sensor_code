# The MIT License (MIT)
# Copyright (c) 2016 Jeremy VanDerWal
# https://opensource.org/licenses/MIT
# 
# Publish data to Thingspeak channels using the MQTT protocol
#
# Micropython implementation using the ESP8266 platform
# Micropython version:  esp8266-20161110-v1.8.6.bin
#
# Hardware used:
# - Pycom LoPy running micropython  
#
#
# prerequisites:
# - Thingspeak account
# - Thingspeak channel to receive data
#
# References:
# adapted from https://github.com/MikeTeachman/micropython-thingspeak-mqtt-esp8266/blob/master/mqtt-to-thingspeak.py
#     
 
import machine
import network
import time
import gc
from umqtt.simple import MQTTClient

### connect to local wifi network
yourWifiSSID = "NETGEAR18" 										# <--- replace with your WIFI network name
yourWifiPassword = "YOUR-NETWORK-PWD" 							# <--- replace with your WIFI network password
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
sta_if.connect(yourWifiSSID, yourWifiPassword)
while not sta_if.isconnected():
  pass
  
### connect to Thingspeak using MQTT
myMqttClient = "my-mqtt-client"  								# define a client names -- this can be anything unique
thingspeakIoUrl = "mqtt.thingspeak.com" 						# define thingspeak url
c = MQTTClient(myMqttClient, thingspeakIoUrl, 1883)  			# setup the connection -- it uses unsecure TCP connection
c.connect()														# connect to ThingSpeak

#
# publish temperature and free heap to Thingspeak using MQTT
#
thingspeakChannelId = "YOUR-CHANNEL-ID"  # <--- replace with your Thingspeak Channel ID
thingspeakChannelWriteapi = "YOUR-CHANNEL-WRITEAPIKEY" # <--- replace with your Thingspeak Write API Key
publishPeriodInSec = 30 
while True:
  ###
  # collect some data and put code here
  #
  ###
  
  # note:  string concatenations below follow best practices as described in micropython reference doc
  credentials = "channels/{:s}/publish/{:s}".format(thingspeakChannelId, thingspeakChannelWriteapi)  
  payload = "field1={:.1f}&field2={:d}\n".format(tempInDegC, gc.mem_free())
  c.publish(credentials, payload)
  
  time.sleep(publishPeriodInSec)
  
c.disconnect() 


##############################################################################
from network import WLAN
from mqtt import MQTTClient
import machine
import time

def settimeout(duration): 
    pass

wlan = WLAN(mode=WLAN.STA)
wlan.antenna(WLAN.EXT_ANT)
wlan.connect("NETGEAR18", auth=(WLAN.WPA2, "FamilyJADE"), timeout=5000)

while not wlan.isconnected(): 
     machine.idle()

print("Connected to Wifi\n")

client = MQTTClient("demo", "mqtt.thingspeak.com", port=1883)
client.settimeout = settimeout
client.connect()

#
# publish temperature and free heap to Thingspeak using MQTT
#
thingspeakChannelId = "128609"  # <--- replace with your Thingspeak Channel ID
thingspeakChannelWriteapi = "R73HJN1DE9YPRD4V" # <--- replace with your Thingspeak Write API Key
publishPeriodInSec = 30 
while True:
  ###
  # collect some data and put code here
  #
  ###
  
  # note:  string concatenations below follow best practices as described in micropython reference doc
  credentials = "channels/{:s}/publish/{:s}".format(thingspeakChannelId, thingspeakChannelWriteapi)  
  payload = "field1={:.1f}&field2={:d}\n".format(21.3, 100)
  client.publish(credentials, payload)
  
  time.sleep(publishPeriodInSec)
  
client.disconnect() 