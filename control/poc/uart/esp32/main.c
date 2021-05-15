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

// Data format: "KEYMESSAGE" (no delimiter between key and message but key must be one byte)
enum Key {
    NUMBER = 0,
    MSG = 1,
};

void init() 
{
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // not supported by standard Arduino
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // Not using a buffer
    uart_driver_install(UART_NUM_1, SERIAL_SIZE_RX, 0, 0, NULL, 0);
}

static void rx_task(void *arg)
{
    uint8_t* data = (uint8_t*) malloc(SERIAL_SIZE_RX+1);
    while (1) {
        int rxBytes = uart_read_bytes(UART_NUM_1, data, SERIAL_SIZE_RX, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            char string[rxBytes+1];
            memcpy(string, data, rxBytes+1);
            string[rxBytes] = 0;
            printf("%s",(char*)string);

            // Send string data
            char* tx_string = "A message from ESP32\n";
            uint8_t stringKey = (uint8_t)MSG;
            uart_write_bytes(UART_NUM_1, (const char*)&stringKey, 1); // send key
            uart_write_bytes(UART_NUM_1, (const char*)tx_string, strlen(tx_string)); // send data

            // Can only send one of them at once for the current system to work

            // Send numeric data
            // uint8_t num = 123;
            // uint8_t numericKey = (uint8_t)NUMBER;
            // uart_write_bytes(UART_NUM_1, (const char*)&numericKey, 1); // send key
            // uart_write_bytes(UART_NUM_1, (const char*)&num, 1); // send data

            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    free(data);
}

void app_main(void)
{
    init();
    printf("\n Start testing\n");
    xTaskCreate(rx_task, "uart_rx_task", 10000, NULL, 1, NULL);
}
