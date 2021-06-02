#include "drive.h"

static const char *DRIVE_tag = "DRIVE";

const DriveEncoding driveEncoding = {"F", "B", "R", "L", "S"};

// Used for calculating backward distance after forced stop
char* currentDriveInstruction = "";
int currentDriveInstructionValue = 0;

// Global FIFO queue used for drive instruction sequence
QueueHandle_t driveInstructionQueue;

void drive_init() {
    driveInstructionQueue = xQueueCreate(MAX_DRIVE_INSTRUCTION_SEQUENCE_LENGTH, sizeof(char[32]));
    if (driveInstructionQueue == 0) {
        ESP_LOGE(DRIVE_tag, "Failed to create drive instruction queue");
        abort();
    }

    ESP_LOGI(DRIVE_tag, "Drive setup completed");
}

void flush_drive_instruction_queue() {
    char* data = (char*) malloc(32*8);

    for (int i=0; i<MAX_DRIVE_INSTRUCTION_SEQUENCE_LENGTH; i++) {
        xQueueReceive(driveInstructionQueue, data, (TickType_t)0);
    }

    free(data);

    ESP_LOGI(DRIVE_tag, "Finished flashing drive instruction queue");
}

void drive_backwards_to_last_valid_tile(int distanceDrivenOfLastInstruction) {
    if (!(strcmp(currentDriveInstruction, driveEncoding.forward) == 0)) {
        ESP_LOGE(DRIVE_tag, "Current instruction not forward, can't calculate distance to drive backwards: %s", currentDriveInstruction);
        return;
    }

    int backwardDistance = distanceDrivenOfLastInstruction % MAP_TILE_WIDTH;

    char encodedData[32];
    sprintf(encodedData, "%s%s%d", driveEncoding.backward, DRIVE_INSTRUCTION_DELIMITER, backwardDistance);
    xQueueSend(driveInstructionQueue, (void*)encodedData, (TickType_t)0);
}
