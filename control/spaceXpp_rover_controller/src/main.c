/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "mqtt.h"
#include "wifi.h"
#include "uart.h"
#include "spi.h"
#include "drive.h"

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

    drive_init();

    mqtt_init();

    drive_uart_init();

    energy_uart_init();

    vision_spi_init();

    xTaskCreate(drive_uart_task, "drive_uart_task", 2048, NULL, 10, NULL);
    // xTaskCreate(drive_uart_task_simulated, "drive_uart_task_simulated", 2048, NULL, 10, NULL);

    xTaskCreate(energy_uart_task, "energy_uart_task", 2048, NULL, 5, NULL);

    xTaskCreate(vision_spi_task, "vision_spi_task", 16384, NULL, 15, NULL);
    // xTaskCreate(vision_spi_task_simulated, "vision_spi_task_simulated", 16384, NULL, 11, NULL);
}
