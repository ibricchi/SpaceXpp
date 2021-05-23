#include <stdio.h>
#include "string.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#ifndef UART_H
#define UART_H

#define DRIVE_UART_NUM UART_NUM_0
#define DRIVE_TXD_PIN (GPIO_NUM_17)
#define DRIVE_RXD_PIN (GPIO_NUM_16)
#define DRIVE_BUFFER_SIZE (256)

#define VISION_UART_NUM UART_NUM_1
// #define VISION_TXD_PIN ???
// #define VISION_RXD_PIN ???
// #define DRIVE_BUFFER_SIZE (???)

#define ENERGY_UART_NUM UART_NUM_2
// #define VISION_TXD_PIN ???
// #define VISION_RXD_PIN ???
// #define DRIVE_BUFFER_SIZE (???)

// Data encodings
typedef struct{
    const char* forward;
    const char* turn;
} DriveEncoding;

// Full-duplex
void drive_uart_init();

// Full-duplex
void vision_uart_init();

// Half-duplex
void energy_uart_init();

// Simple receiver task => Change to UART event based task if this is not sufficient
void drive_uart_task(void *arg);

void vision_uart_task(void *arg);

void energy_uart_task(void *arg);

void send_drive_uart_data(const char* encoding, const char* data);

#endif
