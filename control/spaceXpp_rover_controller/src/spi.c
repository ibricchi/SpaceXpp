#include "spi.h"

static const char *SPI_tag = "SPI";

extern const DriveEncoding driveEncoding;
extern char* currentDriveInstruction;

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
    ESP_ERROR_CHECK(gpio_set_pull_mode(VISION_MOSI_PIN, GPIO_PULLUP_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(VISION_SCLK_PIN, GPIO_PULLUP_ONLY));
    ESP_ERROR_CHECK(gpio_set_pull_mode(VISION_CS_PIN, GPIO_PULLUP_ONLY));

    ESP_ERROR_CHECK(spi_slave_initialize(VISION_SPI_HOST, &buscfg, &slvcfg, VISION_DMA_CHAN));

    ESP_LOGI(SPI_tag, "Vision SPI setup completed");
}

void vision_spi_task(void *arg) {
    WORD_ALIGNED_ATTR char tx_buff[VISION_BUFFER_SIZE]="";
    WORD_ALIGNED_ATTR char rx_buff[VISION_BUFFER_SIZE]="";
    memset(rx_buff, 0, VISION_BUFFER_SIZE);

    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));

    while (1) {
        // Clear rx_buff
        memset(rx_buff, 0, VISION_BUFFER_SIZE);
        // Set tx_buff
        sprintf(tx_buff, "Message from ESP32");

        // Set up a transaction of 128 bytes to send/receive
        t.length=128*8;
        t.tx_buffer=tx_buff;
        t.rx_buffer=rx_buff;

        // Blocking operations until data received
        ESP_ERROR_CHECK(spi_slave_transmit(VISION_SPI_HOST, &t, portMAX_DELAY));

        // Pop key from received data
        const char key = rx_buff[strlen(rx_buff)-1];
        rx_buff[strlen(rx_buff)-1] = '\0';
        switch (key) {
            case 'S': // Stop
                handle_vision_stop_instruction(rx_buff);
                ESP_LOGI(SPI_tag, "Stop instruction from vision: %s", rx_buff);
                break;
            case 'I': // Image data
                break;
            case 'B': // Begin new image
                break;
            case 'E': // End current image
                break;
            default:
                ESP_LOGE(SPI_tag, "Invalid key from vision data: %d", key);
                break;
        }

        printf("\nReceived from FPGA: %s\n", rx_buff); // testing only
    }
}

void vision_spi_task_simulated(void *arg) {
    WORD_ALIGNED_ATTR char rx_buff[VISION_BUFFER_SIZE]="";
    memset(rx_buff, 0, VISION_BUFFER_SIZE);

    while (1) {
        // Clear rx_buff
        memset(rx_buff, 0, VISION_BUFFER_SIZE);

        switch (esp_random() % 7) {
            case 0: // STOP: General obstruction in field before rover
                strcpy(rx_buff, "US");
                break;
            case 1:  // STOP: Blue ball in field before rover
                strcpy(rx_buff, "BS");
                break;
            case 2:  // STOP: Red ball in field before rover
                strcpy(rx_buff, "RS");
                break;
            case 3:
            case 4:
            case 5:
            case 6: // Nothing detected => No data from vision
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            default:
                ESP_LOGE(SPI_tag, "Vision simulation: Random number not in allowed range.");
                break;
        }

        // Pop key from vision data
        const char key = rx_buff[strlen(rx_buff)-1];
        rx_buff[strlen(rx_buff)-1] = '\0';
        switch (key) {
            case 'S': // Stop
                ESP_LOGI(SPI_tag, "Simulated stop instruction: %s", rx_buff);
                handle_vision_stop_instruction(rx_buff);
                break;
            case 'I': // Image data
                break;
            case 'B': // Begin new image
                break;
            case 'E': // End current image
                break;
            default:
                ESP_LOGE(SPI_tag, "Vision simulation: Invalid key from vision data.");
                break;
        }

        // Delay simulates blocking behaviour of "spi_slave_transmit"
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void handle_vision_stop_instruction(char* stopInformation) {
    // Inform server of stop to allow for map update
    publish_drive_instruction_to_server(driveEncoding.stop, stopInformation);

    // Only need to stop if moving forward
    if (strcmp(currentDriveInstruction, driveEncoding.forward) != 0) {
        return;
    }

    currentDriveInstruction = driveEncoding.stopFromForward;

    // Send stop instruction to drive
    send_drive_uart_data(driveEncoding.stop, "0");

    // Don't send any of remaining instructions in queue to drive
    flush_drive_instruction_queue();
}
