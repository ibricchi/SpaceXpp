/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#include "uart.h"

static const char *UART_tag = "UART";

extern const DriveEncoding driveEncoding;
extern QueueHandle_t driveInstructionQueue;
extern char* currentDriveInstruction;
extern int currentDriveInstructionValue;

extern const EnergyEncoding energyEncoding;

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

void energy_uart_init()
{
    const uart_config_t energy_uart_config = {
        .baud_rate = 115200, // Arduino could go higher than this (see datasheet)
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, // not supported by standard Arduino
    };
    ESP_ERROR_CHECK(uart_param_config(ENERGY_UART_NUM, &energy_uart_config));

    ESP_ERROR_CHECK(uart_set_pin(ENERGY_UART_NUM, ENERGY_TXD_PIN, ENERGY_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // No tx buffer as not planning to transmitt data from esp32 to energy Arduino
    ESP_ERROR_CHECK(uart_driver_install(ENERGY_UART_NUM, ENERGY_BUFFER_SIZE, 0, 0, NULL, 0));

    ESP_LOGI(UART_tag, "Energy UART setup completed");
}

void drive_uart_task(void *arg)
{
    static bool driveIsReady = false;

    uint8_t* rx_data = (uint8_t*) malloc(DRIVE_BUFFER_SIZE);
    char* queue_data = (char*) malloc(32*8);

    while (1) {
        int rxBytes = uart_read_bytes(DRIVE_UART_NUM, rx_data, DRIVE_BUFFER_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            rx_data[rxBytes] = 0; // End of received string
            ESP_LOGI(UART_tag, "UART data from drive: %s", (char*)rx_data);

            const char instruction = rx_data[strlen((char*)rx_data)-1];
            rx_data[strlen((char*)rx_data)-1] = '\0';
            switch (instruction) {
                case 'R': // Drive is ready for next instruction
                    driveIsReady = true;
                    break;
                case 'S': // Stop: Distance of last forward instruction driven
                    drive_backwards_to_last_valid_tile(atoi((char*)rx_data));
                    publish_drive_instruction_to_server("SD", (char*)rx_data); // second part of stop instruction for server (required for server to stop on correct tile)
                    break;
                default:
                    ESP_LOGE(UART_tag, "Instruction from drive not valid: %d", instruction);
                    break;
            }
        }

        // Don't block if queue is empty, only send next instruction if drive is ready
        if (driveIsReady && xQueueReceive(driveInstructionQueue, queue_data, (TickType_t)0)) {
            // Check if end of instruction sequence signal (used by server for updating map)
            if (strcmp(queue_data, "X") == 0) {
                publish_drive_instruction_to_server(queue_data, "0");
                currentDriveInstruction = "";
                continue;
            }

            // Decode drive instruction data
            char* instruction = strtok(queue_data, DRIVE_INSTRUCTION_DELIMITER);
            char* value = strtok(NULL, DRIVE_INSTRUCTION_DELIMITER);

            char* encodedInstruction = "";
            if (strcmp(instruction, "F") == 0 || strcmp(instruction, "forward") == 0) {
                encodedInstruction = driveEncoding.forward;
            } else if (strcmp(instruction, "B") == 0 || strcmp(instruction, "backward") == 0) {
                encodedInstruction = driveEncoding.backward;
            } else if (strcmp(instruction, "R") == 0 || strcmp(instruction, "turnRight") == 0) {
                encodedInstruction = driveEncoding.turnRight;
            } else if (strcmp(instruction, "L") == 0 || strcmp(instruction, "turnLeft") == 0) {
                encodedInstruction = driveEncoding.turnLeft;
            } else {
                ESP_LOGE(UART_tag, "Unknown drive instruction: %s", instruction);
            }

            // Send instruction to drive
            send_drive_uart_data(encodedInstruction, value);
            driveIsReady = false;

            // Send instruction to server for updating map
            publish_drive_instruction_to_server(encodedInstruction, value);

            // Update current instruction
            currentDriveInstruction = encodedInstruction;
            currentDriveInstructionValue = atoi(value);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    free(rx_data);
    free(queue_data);
}

void drive_uart_task_simulated(void *arg) {
    static bool driveIsReady = false;

    uint8_t* rx_data = (uint8_t*) malloc(DRIVE_BUFFER_SIZE);
    char* queue_data = (char*) malloc(32*8);

    while (1) {
        // Clear rx_data
        memset(rx_data, 0, DRIVE_BUFFER_SIZE);

        if (strcmp(currentDriveInstruction, driveEncoding.stopFromForward) == 0) {
            int randomDistance = esp_random() % currentDriveInstructionValue + 1; // currentDriveInstructionValue still corresponds to previous (forward) instruction
            sprintf((char*)rx_data, "%d%s", randomDistance, "S");
        } else {
            switch (esp_random() % 5) {
                case 0: // Ready for next instruction
                    strcpy((char*)rx_data, "R");
                    break;
                case 1: // Not yet ready for next instruction => No data from drive
                case 2:
                case 3:
                case 4:
                    break;
                default:
                    ESP_LOGE(UART_tag, "Drive simulation: Random number not in allowed range.");
                    break;
            }
        }

        // simulated receiving from UART
        int rxBytes = strlen((char*)rx_data);
        if (rxBytes > 0) {
            rx_data[rxBytes] = '\0'; // End of received string

            ESP_LOGI(UART_tag, "Simulated data from drive: %s", (char*)rx_data);

            const char instruction = rx_data[strlen((char*)rx_data)-1];
            rx_data[strlen((char*)rx_data)-1] = '\0';
            switch (instruction) {
                case 'R': // Drive is ready for next instruction
                    driveIsReady = true;
                    break;
                case 'S': // Stop: Distance of last forward instruction driven
                    drive_backwards_to_last_valid_tile(atoi((char*)rx_data));
                    publish_drive_instruction_to_server("SD", (char*)rx_data); // second part of stop instruction for server (required for server to stop on correct tile)
                    currentDriveInstruction = ""; // needed in simulation to prevent drive from sending stop distance multiple times
                    break;
                default:
                    ESP_LOGE(UART_tag, "Simulated instruction from drive not valid: %d", instruction);
                    break;
            }
        }

        // Don't block if queue is empty, only send next instruction if drive is ready
        if (driveIsReady && xQueueReceive(driveInstructionQueue, queue_data, (TickType_t)0)) {
            // Check if end of instruction sequence signal (used by server for updating map)
            if (strcmp(queue_data, "X") == 0) {
                publish_drive_instruction_to_server(queue_data, "0");
                currentDriveInstruction = "";
                continue;
            }
            
            // Decode drive instruction data
            char* instruction = strtok(queue_data, DRIVE_INSTRUCTION_DELIMITER);
            char* value = strtok(NULL, DRIVE_INSTRUCTION_DELIMITER);

            char* encodedInstruction = "";
            if (strcmp(instruction, "F") == 0 || strcmp(instruction, "forward") == 0) {
                encodedInstruction = driveEncoding.forward;
            } else if (strcmp(instruction, "B") == 0 || strcmp(instruction, "backward") == 0) {
                encodedInstruction = driveEncoding.backward;
            } else if (strcmp(instruction, "R") == 0 || strcmp(instruction, "turnRight") == 0) {
                encodedInstruction = driveEncoding.turnRight;
            } else if (strcmp(instruction, "L") == 0 || strcmp(instruction, "turnLeft") == 0) {
                encodedInstruction = driveEncoding.turnLeft;
            } else {
                ESP_LOGE(UART_tag, "Unknown simulated drive instruction: %s", instruction);
            }

            // Send instruction to drive
            send_drive_uart_data(encodedInstruction, value);
            driveIsReady = false;

            // Send instruction to server for updating map
            publish_drive_instruction_to_server(encodedInstruction, value);

            // Update current instruction
            currentDriveInstruction = encodedInstruction;
            currentDriveInstructionValue = atoi(value);
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    free(rx_data);
    free(queue_data);
}

void energy_uart_task(void *arg)
{
    uint8_t* rx_data = (uint8_t*) malloc(ENERGY_BUFFER_SIZE);

    while (1) {
        int rxBytes = uart_read_bytes(ENERGY_UART_NUM, rx_data, ENERGY_BUFFER_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            rx_data[rxBytes] = 0; // End of received string
            ESP_LOGI(UART_tag, "UART data from energy: %s", (char*)rx_data);

            const char instruction = rx_data[strlen((char*)rx_data)-1];
            rx_data[strlen((char*)rx_data)-1] = '\0';
            switch (instruction) {
                case 'C': // State of charge battery status
                    publish_energy_status_to_server(energyEncoding.stateOfCharge, (char*)rx_data);
                    break;
                case 'H': // State of health battery status
                    publish_energy_status_to_server(energyEncoding.stateOfHealth, (char*)rx_data);
                    break;
                case 'E': // Error in battery cells
                    publish_energy_status_to_server(energyEncoding.cellError, "0");
                    break;
                default:
                    ESP_LOGE(UART_tag, "Instruction from energy not valid: %d", instruction);
                    break;
            }
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    free(rx_data);
}

void send_drive_uart_data(const char* encoding, const char* data) {
    char tx_string[strlen(data)+strlen(encoding)+3];
    sprintf(tx_string, "<%s%s>", data, encoding);

    uart_write_bytes(DRIVE_UART_NUM, (const char*)tx_string, strlen(tx_string));

    ESP_LOGI(UART_tag, "Drive data sent: %s", tx_string);
}
