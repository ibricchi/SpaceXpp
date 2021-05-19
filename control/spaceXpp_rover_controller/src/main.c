#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "mqtt.h"
#include "wifi.h"
#include "uart.h"

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_sta();

    esp_mqtt_client_handle_t client = mqtt_init();

    drive_uart_init();
    // vision_uart_init();
    // energy_uart_init();

    xTaskCreate(mqtt_task, "mqtt_task", 2048, (void*)client, 10, NULL);

    xTaskCreate(drive_uart_task, "drive_uart_task", 2048, NULL, 10, NULL);
    // xTaskCreate(vision_uart_task, "vision_uart_task", 2048, NULL, 10, NULL);
    // xTaskCreate(energy_uart_task, "energy_uart_task", 2048, NULL, 5, NULL);
}
