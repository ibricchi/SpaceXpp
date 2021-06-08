import ssl
import time
import paho.mqtt.client as mqtt

#define callbacks
def on_message(client, userdata, message):
  print("received message =",str(message.payload.decode("utf-8")))

def on_log(client, userdata, level, buf):
  print("log: ",buf)

def on_connect(client, userdata, flags, rc):
  print("publishing ")
  client.publish("muthu","muthupavithran",)

broker_url = "18.130.239.157"
broker_port = 8883

client = mqtt.Client()

client.on_message=on_message
client.on_log=on_log
client.on_connect=on_connect

client.username_pw_set("<username>", "<pass>")
client.tls_set("..\..\control\spaceXpp_rover_controller\src\ca_cert.pem", tls_version=ssl.PROTOCOL_TLSv1_2)
client.tls_insecure_set(True)

if client.connect(broker_url, broker_port, 60) != 0:
    print("Couldn't connect")
    exit(-1)

client.publish("testing/hello", "Hello World from paho-mqtt!", 0)

client.disconnect()