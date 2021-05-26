#!/bin/bash

set -eou pipefail

HTTP_PORT="${1:-3000}"
MQTT_BROKER_URL="${2:-ssl://18.130.239.157:8883}"
DB_NAME="${3:-serverDB.db}"

mkdir -p bin db

make server

./bin/server -httpPort ${HTTP_PORT} -mqttBrokerURL ${MQTT_BROKER_URL} -db ${DB_NAME}
