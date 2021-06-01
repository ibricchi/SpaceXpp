#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#ifndef WIFI_H
#define WIFI_H

// Must insert WiFi data here
#define WIFI_SSID      "???"
#define WIFI_PASS      "???"

#define WIFI_MAXIMUM_RETRY  10

/* 
    The event group allows multiple bits for each event, but we only care about two events:
    - we are connected to the AP with an IP
    - we failed to connect after the maximum amount of retries 
*/
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void wifi_init_sta();

#endif
