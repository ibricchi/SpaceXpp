# WiFi

## Add WiFi credentials

This must be done before uploading. It is best to mask the WiFi credentials using `"???"` or similar before pushing.

The WiFi credentials can be found in `spaceXpp_rover_controller/include/wifi.h` and are defined using `ESP_WIFI_SSID` and `ESP_WIFI_PASS`.

# MQTT

## Mosquitto Setup

### Setup Mosquitto broker on cloud

```
sudo apt-get update
sudo apt-get install mosquitto
```

Service is started automatically after installation. Use `systemctl` for configuring the service.

For example:
```
systemctl status mosquitto
```

### Setup Mosquitto broker over SSL/TLS

Require SANs certificates as Go no longer supports depreciated common name matching.

Start by copying `mqtt_broker.cnf` to the Mosquitto server and then run the following commands. Make sure, that the `CN=18.130.239.157` attribute matches the IP address of the Mosquitto server.

```bash
# Create the CA key
openssl genrsa -aes256 -out ca.key 2048
chmod 400 ca.key

# Create the CA certificate
openssl req -new -x509 -subj "/C=UK/O=Space CA/CN=18.130.239.157" -extensions v3_ca -days 3650 -key ca.key -sha256 -out ca.crt -config mqtt_broker.cnf

# Check the CA certificate
openssl x509 -in ca.crt -text -noout

# Create the server key
openssl genrsa -out server.key 2048

# Create the server signing request
openssl req -subj "/C=UK/O=SpaceXpp/CN=18.130.239.157" -extensions v3_req -sha256 -new -key server.key -out server.csr

# Sign the server certificate
openssl x509 -req -extensions v3_req -days 3650 -sha256 -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -extfile mqtt_broker.cnf

# Check the server certificate
openssl x509 -in server.crt -text -noout

sudo mv ca.crt /etc/mosquitto/ca_certificates/
sudo mv server.key /etc/mosquitto/certs/
sudo mv server.crt /etc/mosquitto/certs/

sudo systemctl restart mosquitto
```

Now, add the following lines to the `/etc/mosquitto/mosquitto.conf` file:

```
port 8883
cafile /etc/mosquitto/ca_certificates/ca.crt
keyfile /etc/mosquitto/certs/server.key
certfile /etc/mosquitto/certs/server.crt
```

Test if everything is working using the MQTT client tools.

Copy the `ca.crt` to the client.

## MQTT Client Tools

These instructions assume that Mosquitto was setup over SSL/TLS using the instructions above.

### Install MQTT client tools (mosquitto_sub and mosquitto_pub)

```
sudo apt-get update
sudo apt-get install mosquitto-clients
```

### Subscribe to topic using mosquitto_sub

```
mosquitto_sub -h 18.130.239.157 -p 8883 --cafile /etc/mosquitto/ca_certificates/ca.crt -t "/test/test1"
```

### Publish to topic using mosquitto_pub

```
mosquitto_pub -h 18.130.239.157 -p 8883 --cafile /etc/mosquitto/ca_certificates/ca.crt -t "/test/test1" -m "Message to publish"
```
