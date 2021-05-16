#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Aiming to use IO_MUX pins for SPI buses (can avoid overhead of GPIO Matrix)
#define PIN_NUM_MISO (GPIO_NUM_12)
#define PIN_NUM_MOSI (GPIO_NUM_13)
#define PIN_NUM_CLK  (GPIO_NUM_14)
#define PIN_NUM_CS0  (GPIO_NUM_15)

void tx_spi(spi_device_handle_t spi, char *data, int len)
{
    if (len == 0) return; // no data to send

    WORD_ALIGNED_ATTR char recvbuf[100]="";
    memset(recvbuf, 0, 100);

    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // zero out transaction

    t.length = len*8; // convert len from bytes to bits
    t.tx_buffer = data;
    t.rx_buffer = recvbuf;

    // transmit using interrupt (interrupts only make sense for large data due to overhead => would not normally use here)
    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));

    printf("Just received: %s\n", recvbuf);
}

void rx_spi(spi_device_handle_t spi, char *recvbuf)
{
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));

    t.length = 30*8; // Reserve some space for data to be received
    t.rx_buffer = recvbuf;

    ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

    printf("Just received: %s\n", recvbuf);
}

static void spi_task(void *arg)
{
    spi_device_handle_t spi = (spi_device_handle_t)arg;

    while (1) {
        char* data = "A:";
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));       // zero out the transaction
        t.length=2*8;                     // data is 8 bits (does not work if larger/smaller than data length)
        t.tx_buffer=data;
        ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    while (1) {
        // WORD_ALIGNED_ATTR char rx_data[30];
        // memset(rx_data, 0, 30);
        // rx_spi(spi, rx_data);
        // printf("Data received: %s\n", rx_data);

        char* tx_data = "Hello\r";
        tx_spi(spi, tx_data, strlen(tx_data));

        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}

// This works (transmitting one byte works but anything bigger causes problems)
// Receiving data from the Arduino does not yet work
static void simpleSPITest(void* arg) {
    spi_device_handle_t spi = (spi_device_handle_t)arg;

    while (1) {
        uint8_t data = 215;
        // uint8_t rx_data = 0;

        spi_transaction_t t;
        memset(&t, 0, sizeof(t));       // zero out the transaction
        t.length=8;                     // data is 8 bits
        t.tx_buffer=&data;
        // t.rx_buffer=&rx_data;

        ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));

        // printf("Data received: %d\n", rx_data);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    // configure SPI bus
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    // configure slave device timing requirements
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 8*1000*1000,  // clock out at 8 MHz (divisor of 80Mhz)
        .mode = 1,                      // SPI mode 1 (default mode of Arduino) => Might need to change for other devices e.g. FPGA
        .spics_io_num = PIN_NUM_CS0,    // CS pin
        .queue_size = 5,                // amount of transactions that can be queued at any one time
    };

    //Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, 1));

    //Attach slave device to the SPI bus
    spi_device_handle_t spi; // slave device handle
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &spi));

    // xTaskCreate(spi_task, "spi_task", 2000, (void*)spi, 1, NULL);
    xTaskCreate(simpleSPITest, "simpleSPITest", 2000, (void*)spi, 1, NULL);
}
