#include "drive.h"

static const char *DRIVE_tag = "DRIVE";

const DriveEncoding driveEncoding = {"F", "R", "L"};

// Global FIFO queue used for drive instruction sequence
QueueHandle_t driveInstructionQueue;

void drive_init() {
    driveInstructionQueue = xQueueCreate(MAX_DRIVE_INSTRUCTION_SEQUENCE_LENGTH, sizeof(char[32]));
    if (driveInstructionQueue == 0) {
        ESP_LOGE(DRIVE_tag, "Failed to create drive instruction queue");
        abort();
    }
}
