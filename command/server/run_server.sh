#!/bin/bash

set -eou pipefail

HTTP_PORT="${1:-3000}"
HTTP_TLS_CERT_FILE="${2:-cert/server.crt}"
HTTP_TLS_KEY_FILE="${3:-cert/server.key}"
MQTT_BROKER_URL="${4:-ssl://18.130.239.157:8883}"
MQTT_USERNAME="${5:-spaceXpp_server}"
MQTT_PASSWORD="${6:-mqtt}"
DB_NAME="${7:-serverDB.db}"

mkdir -p bin db

make server

./bin/server -httpPort ${HTTP_PORT} -mqttBrokerURL ${MQTT_BROKER_URL} -mqttUsername ${MQTT_USERNAME} -mqttPassword ${MQTT_PASSWORD} -db ${DB_NAME}
