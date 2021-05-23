#include "esp_log.h"
#include "mqtt_client.h"

#include "uart.h"

#ifndef MQTT_H
#define MQTT_H

#define MQTT_BROKER_URI "mqtts://18.130.239.157:8883" // Mosquitto MQTT broker hosted on AWS Free Tier

extern const uint8_t mqtt_cert[] asm("_binary_ca_cert_pem_start");

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

esp_mqtt_client_handle_t mqtt_init();

// Used to send status to server
void mqtt_task(void *arg);

#endif
