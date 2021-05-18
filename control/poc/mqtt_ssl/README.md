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

Best to follow this detailed guide: http://www.steves-internet-guide.com/mosquitto-tls/ (I recommend installing `openssl` and then skipping directly to the "Detailed Steps" section).

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
