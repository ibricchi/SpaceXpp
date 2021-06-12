/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#include "mqtt.h"

static const char *MQTT_tag = "MQTT";

esp_mqtt_client_handle_t mqttClient;

extern QueueHandle_t driveInstructionQueue;

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // ESP_LOGI(MQTT_tag, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_CONNECTED");

        subscribe_to_mqtt_topics();

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_DISCONNECTED");

        break;

    case MQTT_EVENT_SUBSCRIBED:
        // ESP_LOGI(MQTT_tag, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        // ESP_LOGI(MQTT_tag, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        
        break;

    case MQTT_EVENT_PUBLISHED:
        // ESP_LOGI(MQTT_tag, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        
        break;

    case MQTT_EVENT_DATA:
        // ESP_LOGI(MQTT_tag, "MQTT_EVENT_DATA");

        handle_mqtt_event_data(event);

        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(MQTT_tag, "MQTT_EVENT_ERROR");

        break;

    default:
        ESP_LOGI(MQTT_tag, "Other event id:%d", event->event_id);
        
        break;
    }
}

void mqtt_init()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
        .cert_pem = (const char*)mqtt_cert, // Used for TLS/SSL
        .skip_cert_common_name_check = true, // Required as esp32 seems to use common name instead of SANs, even though this was depreciated in 2000
        .username = (const char*)MQTT_USERNAME,
        .password = (const char*)MQTT_PASSWORD,
    };
    mqttClient = esp_mqtt_client_init(&mqtt_cfg);

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqttClient, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqttClient));

    ESP_LOGI(MQTT_tag, "MQTT setup completed");
}

void subscribe_to_mqtt_topics() {
    int msg_id = esp_mqtt_client_subscribe(mqttClient, "/drive/instruction", 2);
    ESP_LOGI(MQTT_tag, "sent subscribe successful for path '/drive/instruction', msg_id=%d", msg_id);
}

void handle_mqtt_event_data(esp_mqtt_event_handle_t event) {
    char topic[32];
    sprintf(topic, "%.*s", event->topic_len, event->topic);
    char data[32];
    sprintf(data, "%.*s", event->data_len, event->data);

    if (strcmp(topic, "/drive/instruction") == 0) {
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_DATA: data from topic: %s", topic);

        // Block for 10 ticks if queue is full
        xQueueSend(driveInstructionQueue, (void*)data, (TickType_t)10);
    } else {
        ESP_LOGE(MQTT_tag, "MQTT_EVENT_DATA: Unknown topic: %s", topic);
    }
}

void publish_drive_instruction_to_server(const char* instruction, const char* data) {
    char message[32];
    sprintf(message, "%s%s%s", instruction, DRIVE_INSTRUCTION_DELIMITER, data);

    int msg_id = esp_mqtt_client_publish(mqttClient, "/feedback/instruction", message, 0, 2, 0);
    ESP_LOGI(MQTT_tag, "publish drive instruction to server successful, msg_id=%d, data=%s", msg_id, message);
}

void publish_energy_status_to_server(const char* encoding, const char* data) {
    char message[32];
    sprintf(message, "%s%s%s", encoding, ENERGY_INSTRUCTION_DELIMITER, data);

    int msg_id = esp_mqtt_client_publish(mqttClient, "/energy/status", message, 0, 0, 0);
    ESP_LOGI(MQTT_tag, "publish energy status to server successful, msg_id=%d, data=%s", msg_id, message);
}
