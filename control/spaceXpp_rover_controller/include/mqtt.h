/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#include "esp_log.h"
#include "mqtt_client.h"

#include "drive.h"
#include "energy.h"

#ifndef MQTT_H
#define MQTT_H

#define MQTT_BROKER_URI "mqtts://18.130.239.157:8883" // Mosquitto MQTT broker hosted on AWS Free Tier
#define MQTT_USERNAME "spaceXpp_esp32"
#define MQTT_PASSWORD "mqtt"

extern const uint8_t mqtt_cert[] asm("_binary_ca_cert_pem_start");

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

void mqtt_init();

void subscribe_to_mqtt_topics();

void handle_mqtt_event_data(esp_mqtt_event_handle_t event);

void publish_drive_instruction_to_server(const char* instruction, const char* data);

void publish_energy_status_to_server(const char* encoding, const char* data);

#endif
