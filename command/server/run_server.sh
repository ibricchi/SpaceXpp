#!/bin/bash

set -eou pipefail

MQTT_BROKER_URL="${1:-ssl://18.130.239.157:8883}"

make server

./bin/server -mqttBrokerURL ${MQTT_BROKER_URL}
