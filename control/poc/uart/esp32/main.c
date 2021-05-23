#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"


#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define SERIAL_SIZE_RX 130

// Data encodings (to be appended directly after data)
typedef struct{
    const char* forward;
    const char* turn;
} DriveEncoding;
static const DriveEncoding driveEncoding = {"F", "T"};

void init() 
{
    const uart_config_t uart_config = {
        .baud_rate = 115200, // Arduino could go higher than this (see datasheet)
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // not supported by standard Arduino
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, SERIAL_SIZE_RX, SERIAL_SIZE_RX, 0, NULL, 0));
}

static void rx_task(void *arg)
{
    uint8_t* data = (uint8_t*) malloc(SERIAL_SIZE_RX+1);
    while (1) {
        int rxBytes = uart_read_bytes(UART_NUM_1, data, SERIAL_SIZE_RX, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0; // End of received string
            printf("UART data from drive: %s", (char*)data);

            // Send string data
            char* data = "Message from ESP32\n";
            char tx_string[40];
            sprintf(tx_string, "<%s%s>", data, driveEncoding.forward);
            uart_write_bytes(UART_NUM_1, (const char*)tx_string, strlen(tx_string)); // send data
        }

         vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    free(data);
}

void app_main(void)
{
    init();
    printf("\n Start testing\n");
    xTaskCreate(rx_task, "uart_rx_task", 10000, NULL, 1, NULL);
}
