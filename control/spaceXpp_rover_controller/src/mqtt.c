#include "mqtt.h"

static const char *MQTT_tag = "MQTT";

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(MQTT_tag, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_CONNECTED");

        msg_id = esp_mqtt_client_subscribe(client, "/test/cmd", 0);
        ESP_LOGI(MQTT_tag, "sent subscribe successful, msg_id=%d", msg_id);

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_DISCONNECTED");

        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_DATA");
        
        // TODO: Do processing on received data
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT_tag, "MQTT_EVENT_ERROR");

        break;

    default:
        ESP_LOGI(MQTT_tag, "Other event id:%d", event->event_id);
        
        break;
    }
}

esp_mqtt_client_handle_t mqtt_init()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URI,
        .cert_pem = (const char*)mqtt_cert, // Used for TLS/SSL
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));

    return client;
}

// Used to send status to server
void mqtt_task(void *arg)
{
    esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)arg;

    int counter = 0;
    int msg_id;
    while (1) {
        char data[20];
        sprintf(data, "Counter is %d", counter);

        msg_id = esp_mqtt_client_publish(client, "/test/status", data, 0, 0, 0);
        ESP_LOGI(MQTT_tag, "sent publish successful, msg_id=%d, data=%s", msg_id, data);

        ++counter;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
