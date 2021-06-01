#include "uart.h"

static const char *UART_tag = "UART";

extern const DriveEncoding driveEncoding;
extern QueueHandle_t driveInstructionQueue;

void drive_uart_init()
{
    const uart_config_t drive_uart_config = {
        .baud_rate = 115200, // Arduino could go higher than this (see datasheet)
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // not supported by standard Arduino
    };
    ESP_ERROR_CHECK(uart_param_config(DRIVE_UART_NUM, &drive_uart_config));

    ESP_ERROR_CHECK(uart_set_pin(DRIVE_UART_NUM, DRIVE_TXD_PIN, DRIVE_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(DRIVE_UART_NUM, DRIVE_BUFFER_SIZE, DRIVE_BUFFER_SIZE, 0, NULL, 0));

    ESP_LOGI(UART_tag, "Drive UART setup completed");
}

void vision_uart_init()
{
    // TODO: implement based on DE10-Lite UART interface
}

void energy_uart_init()
{
    // TODO: implement based on DE10-Lite UART interface
}

void drive_uart_task(void *arg)
{
    static bool driveIsReady = false;

    uint8_t* rx_data = (uint8_t*) malloc(DRIVE_BUFFER_SIZE+1);
    char* queue_data = (char*) malloc(32*8);

    while (1) {
        int rxBytes = uart_read_bytes(DRIVE_UART_NUM, rx_data, DRIVE_BUFFER_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            rx_data[rxBytes] = 0; // End of received string
            printf("UART data from drive: %s\n", (char*)rx_data);

            char quickInstruction = rx_data[0];
            if (quickInstruction == 'R') { // Drive is ready for next instruction
                driveIsReady = true;
            }
        }

        // Don't block if queue is empty, only send next instruction if drive is ready
        if (driveIsReady && xQueueReceive(driveInstructionQueue, queue_data, (TickType_t)0)) {
            // Decode drive instruction data
            char* instruction = strtok(queue_data, DRIVE_INSTRUCTION_DELIMITER);
            char* value = strtok(NULL, DRIVE_INSTRUCTION_DELIMITER);

            char* encodedInstruction = "";
            if (strcmp(instruction, "F") || strcmp(instruction, "forward")) {
                encodedInstruction = driveEncoding.forward;
            } else if (strcmp(instruction, "R") || strcmp(instruction, "turnRight")) {
                encodedInstruction = driveEncoding.turnRight;
            } else if (strcmp(instruction, "L") || strcmp(instruction, "turnLeft")) {
                encodedInstruction = driveEncoding.turnLeft;
            } else {
                ESP_LOGE(UART_tag, "Unknown drive instruction: %s", instruction);
            }

            // Send instruction to drive
            send_drive_uart_data(encodedInstruction, value);
            driveIsReady = false;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    free(rx_data);
    free(queue_data);
}

void vision_uart_task(void *arg)
{
    while (1) {
        // TODO: implement
    }
}

void energy_uart_task(void *arg)
{
    while (1) {
        // TODO: implement
    }
}

void send_drive_uart_data(const char* encoding, const char* data) {
    char tx_string[strlen(data)+strlen(encoding)+3];
    sprintf(tx_string, "<%s%s>", data, encoding);

    uart_write_bytes(DRIVE_UART_NUM, (const char*)tx_string, strlen(tx_string));

    ESP_LOGI(UART_tag, "Drive data sent: %s", tx_string);
}
