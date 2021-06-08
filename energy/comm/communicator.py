import ssl
from sys import stdin
import paho.mqtt.client as mqtt

#define callbacks
def on_message(client, userdata, message):
  print("received message =",str(message.payload.decode("utf-8")))

def on_log(client, userdata, level, buf):
  print("log: ",buf)

broker_url = "18.130.239.157"
broker_port = 8883

client = mqtt.Client()

# client.on_message=on_message
# client.on_log=on_log

client.username_pw_set("<username>", "<pass>")
client.tls_set("../../control/spaceXpp_rover_controller/src/ca_cert.pem", tls_version=ssl.PROTOCOL_TLSv1_2)
client.tls_insecure_set(True)

if client.connect(broker_url, broker_port, 60) != 0:
    print("Couldn't connect")
    exit(-1)

def on_exit():
    # print("exited")
    client.disconnect()

import atexit
atexit.register(on_exit)

for line in stdin:
    i = line
    t = i[-2:-1]
    v = i[:-2]
    m = f'{t}:{v}'
    print(m)
    # client.publish("testing/hello", "Hello World from paho-mqtt!", 0)

