#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#ifndef DRIVE_H
#define DRIVE_H

#define MAX_DRIVE_INSTRUCTION_SEQUENCE_LENGTH (50)
#define DRIVE_INSTRUCTION_DELIMITER ":"

// UART data encoding
typedef struct DriveEncoding{
    const char* forward;
    const char* turnRight;
    const char* turnLeft;
} DriveEncoding;

void drive_init();

#endif
