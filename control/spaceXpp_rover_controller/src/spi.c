#include "spi.h"

static const char *SPI_tag = "SPI";

void vision_spi_init() {
    const spi_bus_config_t buscfg = {
        .mosi_io_num = VISION_MOSI_PIN,
        .miso_io_num = VISION_MISO_PIN,
        .sclk_io_num = VISION_SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    const spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = VISION_CS_PIN,
        .queue_size = 5, // number of transactions that can be queued
        .flags = 0,
    };

    // Prevent detection of pulses when no master is connected
    gpio_set_pull_mode(VISION_MOSI_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(VISION_SCLK_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(VISION_CS_PIN, GPIO_PULLUP_ONLY);

    ESP_ERROR_CHECK(spi_slave_initialize(VISION_SPI_HOST, &buscfg, &slvcfg, VISION_DMA_CHAN));
}

void vision_spi_task(void *arg) {
    WORD_ALIGNED_ATTR char tx_buff[129]="";
    WORD_ALIGNED_ATTR char rx_buff[129]="";
    memset(rx_buff, 0, 129);

    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    while (1) {
        // Clear rx_buff
        memset(rx_buff, 0, 129);
        // Set tx_buff
        sprintf(tx_buff, "Message from ESP32");

        // Set up a transaction of 128 bytes to send/receive
        t.length=128*8;
        t.tx_buffer=tx_buff;
        t.rx_buffer=rx_buff;

        // Blocking operations until data received
        ESP_ERROR_CHECK(spi_slave_transmit(VISION_SPI_HOST, &t, portMAX_DELAY));

        printf("\nReceived from FPGA: %s\n", rx_buff);
    }
}
