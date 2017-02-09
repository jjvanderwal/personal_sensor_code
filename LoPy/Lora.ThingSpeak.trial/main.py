from network import WLAN
from simple import MQTTClient
import machine
import time

import socket
import struct
from network import LoRa

#print(b'1000,999'.split(b','))

###start the wifi
wlan = WLAN(mode=WLAN.STA)
wlan.connect("NETGEAR18", auth=(WLAN.WPA2, "FamilyJADE"), timeout=5000)
while not wlan.isconnected(): 
     machine.idle()
print("Connected to Wifi\n")

###define the thingspeak details
client = MQTTClient("demo", "mqtt.thingspeak.com", port=1883)
client.connect()
thingspeakChannelId = "128609"  # <--- replace with your Thingspeak Channel ID
thingspeakChannelWriteapi = "R73HJN1DE9YPRD4V" # <--- replace with your Thingspeak Write API Key
publishPeriodInSec = 30 
print("client connected\n")

###setup the lora gateway
_LORA_PKG_FORMAT = "!BB%ds"  # A basic package header, B: 1 byte for the deviceId, B: 1 byte for the pkg size, %ds: Formated string for string
_LORA_PKG_ACK_FORMAT = "BBB"  # A basic ack package, B: 1 byte for the deviceId, B: 1 bytes for the pkg size, B: 1 byte for the Ok (200) or error messages

###Open a LoRa Socket, use rx_iq to avoid listening to our own messages
lora = LoRa(mode=LoRa.LORA, rx_iq=True)
lora_sock = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
lora_sock.setblocking(False)

while (True):
    recv_pkg = lora_sock.recv(512)
    if (len(recv_pkg) > 2):
        recv_pkg_len = recv_pkg[1]
        device_id, pkg_len, msg = struct.unpack(_LORA_PKG_FORMAT % recv_pkg_len, recv_pkg)
        print('Device: %d - Pkg:  %s' % (device_id, msg)) # If the uart = machine.UART(0, 115200) and os.dupterm(uart) are set in the boot.py this print should appear in the serial port
        ack_pkg = struct.pack(_LORA_PKG_ACK_FORMAT, device_id, 1, 200)
        lora_sock.send(ack_pkg)
        # note:  string concatenations below follow best practices as described in micropython reference doc
        credentials = "channels/{:s}/publish/{:s}".format(thingspeakChannelId, thingspeakChannelWriteapi)  
        msg = msg.decode('UTF-8')  #convert to a string
        msg = msg.split(',')
        f1 = float(msg[0])
        f2 = float(msg[1])
        print(msg)
        payload = "field1={:.1f}&field2={:.1f}\n".format(f1, f2)
        print(payload)
        client.publish(credentials, payload)

  
#client.disconnect() 
