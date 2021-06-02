#include "wifi.h"

static const char *Wifi_tag = "WIFI";

// FreeRTOS event group to signal when we are connected
static EventGroupHandle_t wifiEventGroup;

// Keeping track of connection retries
static int wifiRetries = 0;

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_ERROR_CHECK(esp_wifi_connect());
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (wifiRetries < WIFI_MAXIMUM_RETRY) {
            ESP_ERROR_CHECK(esp_wifi_connect());
            wifiRetries++;
            
            ESP_LOGI(Wifi_tag, "Retry to connect to the wifi ap");
        } else {
            xEventGroupSetBits(wifiEventGroup, WIFI_FAIL_BIT);
        }

        ESP_LOGI(Wifi_tag,"Connect to the wifi ap failed");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;

        ESP_LOGI(Wifi_tag, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        wifiRetries = 0;
        xEventGroupSetBits(wifiEventGroup, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta()
{
    wifiEventGroup = xEventGroupCreate();

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
	        .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(Wifi_tag, "WIFI setup completed");

    /* 
        Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
        number of re-tries (WIFI_FAIL_BIT). The bits are set by wifi_event_handler()
    */
    EventBits_t bits = xEventGroupWaitBits(wifiEventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    // xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually happened.
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(Wifi_tag, "Connected to wifi ap SSID: %s, password: %s", WIFI_SSID, WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(Wifi_tag, "Failed to connect to wifi ap SSID: %s, password: %s", WIFI_SSID, WIFI_PASS);
    } else {
        ESP_LOGE(Wifi_tag, "UNEXPECTED EVENT");
    }

    // The event will not be processed after unregister
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(wifiEventGroup);
}
