#include <stdio.h>
#include "string.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#include "drive.h"
#include "mqtt.h"

#ifndef UART_H
#define UART_H

#define DRIVE_UART_NUM UART_NUM_1
#define DRIVE_TXD_PIN (GPIO_NUM_17)
#define DRIVE_RXD_PIN (GPIO_NUM_16)
#define DRIVE_BUFFER_SIZE (256)

#define ENERGY_UART_NUM UART_NUM_2
#define ENERGY_TXD_PIN (GPIO_NUM_23)
#define ENERGY_RXD_PIN (GPIO_NUM_22)
#define ENERGY_BUFFER_SIZE (256)

// Full-duplex
void drive_uart_init();

// Half-duplex (Data only needs to be send from energy to control)
void energy_uart_init();

// Simple receiver task => Change to UART event based task if this is not sufficient
void drive_uart_task(void *arg);

/* 
    Simulates drive communication when used instead of "drive_uart_task".
    Used for testing without the drive component being connected.
*/
void drive_uart_task_simulated(void *arg);

void energy_uart_task(void *arg);

void send_drive_uart_data(const char* encoding, const char* data);

#endif
